using Nito.Disposables;
using ParserFramework;
using SemanticMachine.Grammar.Interpretation;
using SemanticMachine.Grammar.Symbols.Decalaration;
using SemanticMachine.Grammar.Symbols.Expression;
using System.Collections.Immutable;
using System.Text;

namespace SemanticMachine.Binding;

public class CppCodeGenerator
{
    public CppLibraryReference LibReference { get; private set; } = new();

    public enum RuntimeRefMode
    {
        This,
        Argument
    }

    public static string SerializeOperation(ArithmeticOperation operation) => operation switch
    {
        ArithmeticOperation.Add => "+",
        ArithmeticOperation.Minus => "-",
        ArithmeticOperation.Mult => "*",
        ArithmeticOperation.Divide => "/",
        ArithmeticOperation.BitwiseAnd => "&",
        ArithmeticOperation.BitwiseOr => "|",
        ArithmeticOperation.LogicAnd => "&&",
        ArithmeticOperation.LogicOr => "||",
        ArithmeticOperation.Equals => "==",
        _ => throw new NotImplementedException()
    };

    public void CompileCpp(ParseTree tree, TextWriter headerFile, TextWriter cppFile)
    {
        // load built-in types
        ImmutableDictionary<string, ISemanticUnit> context = IArithmetic.LoadArithmeticPrimitives(ImmutableDictionary<string, ISemanticUnit>.Empty);
        ParseTree[] flattened = Script.Flatten(tree.Children);

        // load user types
        (ImmutableDictionary<string, ISemanticUnit> newContext, ImmutableList<TypeDefinition> userDefinedTypes) = flattened.Aggregate((context, ImmutableList<TypeDefinition>.Empty), (oldContext, child) => child switch
        {
            ParseTree(TypeDecalaration, ParseTree[] decChildren) => TypeDecalaration.Verify(decChildren, oldContext.context) switch
            {
                TypeDefinition verifiedDef => (oldContext.context.Add(verifiedDef.Name, verifiedDef), oldContext.Empty.Add(verifiedDef))
            },
            _ => oldContext
        });

        context = newContext;

        // load function prototypes
        context = flattened.Aggregate(context, (oldContext, child) => child switch
        {
            ParseTree(MaybeInlineFunction, ParseTree[] decChildren) => MaybeInlineFunction.ExtractHeader(decChildren, oldContext) switch
            {
                FunctionPrototype prototype => oldContext.Add(prototype.Name, prototype)
            },
            _ => oldContext
        });

        // load actual function
        ImmutableDictionary<string, FunctionDefinition> functionDefinitions = flattened.Aggregate(ImmutableDictionary<string, FunctionDefinition>.Empty, (oldContext, child) => child switch
        {
            ParseTree(MaybeInlineFunction, ParseTree[] decChildren) => MaybeInlineFunction.VerifyFullDefinition(decChildren, context) switch
            {
                FunctionDefinition prototype => oldContext.Add(prototype.Prototype.Name, prototype)
            },
            _ => oldContext
        });

        // prime the header and the cpp file
        using IDisposable headerDisposable = LibReference.BeginHeaderFile(headerFile);
        using IDisposable cppDisposable = LibReference.BeginCppFile(cppFile);

        foreach (TypeDefinition def in userDefinedTypes)
        {
            GenerateTypeDef(def, headerFile);
        }

        foreach (var functionDef in functionDefinitions.Values)
        {
            GenerateFunction(functionDef, headerFile, cppFile);
        }
    }

    public void GenerateFunction(FunctionDefinition definition, TextWriter header, TextWriter impl)
    {
        VarNameDistributor dispo = new();

        string prototype = $"{LibReference.PrintType(definition.Prototype.ReturnType)} {definition.Prototype.Name}({string.Join(", ", definition.Prototype.Params.Select(p => $"{LibReference.PrintType(p.Type)} {p.Name}"))});";

        header.WriteLine(prototype);

        using IDisposable implDisposable = ConstructMemeberFunctionDefinition(definition.Prototype, impl);

        StringBuilder bodyBuilder = new();
        bodyBuilder
            .AppendLine($"{LibReference.PrintType(definition.Prototype.ReturnType)} {definition.Prototype.Name}({string.Join(", ", definition.Prototype.Params.Select(p => $"{LibReference.PrintType(p.Type)} {p.Name}"))})")
            .AppendLine("{");

        string value = ProcessEvals(definition.Body, dispo, impl, RuntimeRefMode.This);

        impl.WriteLine($"return {value};");
    }

    public void GenerateTypeDef(TypeDefinition typeDef, TextWriter writer)
    {
        using IDisposable disposable = ConstructTypeDef(typeDef.Name, writer);

        foreach (string paramName in typeDef.PositionalParameters)
        {
            writer.WriteLine($"{LibReference.PrintType(typeDef.Members[paramName])} {paramName};");
        }
    }

    private string ExpandNodeArithmetic(NodeArithmetic node, VarNameDistributor nameDispo, TextWriter writer, RuntimeRefMode mode)
    {
        string valueL = ProcessEvals(node.LeftChild, nameDispo, writer, mode);
        string valueR = ProcessEvals(node.RightChild, nameDispo, writer, mode);

        return $"{valueL} {SerializeOperation(node.Operation)} {valueR}";
    }

    private string ExpandFunctionCall(StaticInvocation call, VarNameDistributor nameDispo, TextWriter writer, RuntimeRefMode mode)
    {
        List<string> parameters = [];
        foreach (IEvaluatable parameter in call.Arguments)
        {
            parameters.Add(ProcessEvals(parameter, nameDispo, writer, mode));
        }

        return $"{call.Function.Name}({string.Join(", ", parameters)})";
    }

    private string ExpandArrayCreation(ArrayLiteral initializer, VarNameDistributor nameDispo, TextWriter writer, RuntimeRefMode mode)
    {
        List<string> elements = [];
        foreach (IEvaluatable element in initializer.Elements)
        {
            elements.Add(ProcessEvals(element, nameDispo, writer, mode));
        }

        return LibReference.CreateArrayEnumerable(LibReference.PrintType(initializer.ElementType), [.. elements], nameDispo, writer, mode switch { RuntimeRefMode.Argument => "runtime", _ => null });
    }

    private string ExpandLambda(LambdaWithCaptures lambda, VarNameDistributor nameDispo, TextWriter writer, RuntimeRefMode mode)
    {
        // create a custom struct just for this lambda
        string lambdaClassName = nameDispo.GetName();
        writer.WriteLine($"struct {lambdaClassName} : public ILambda<{LibReference.PrintType(lambda.Body.Type)}, {string.Join(", ", lambda.Params.Select(cap => LibReference.PrintType(cap.Type)))}>");
        writer.WriteLine("{");

        // print out the captured types
        foreach (LazyEvaluatable capture in lambda.ArgumentCaptures)
        {
            writer.WriteLine($"{LibReference.PrintType(capture.Type)} {capture.Name};");
        }

        // print out the function
        writer.WriteLine($"{LibReference.PrintType(lambda.Body.Type)} Run({string.Join(", ", lambda.Params.Select(param => $"{LibReference.PrintType(param.Type)} {param.Name}").Prepend(LibReference.RuntimeReferenceAsParameter("runtime")))}) override");
        writer.WriteLine("{");

        // the body needs to be generated using a runtime reference instead of 
        string value = ProcessEvals(lambda.Body, nameDispo, writer, RuntimeRefMode.Argument);
        writer.WriteLine($"return {value};");
        writer.WriteLine("}");
        writer.WriteLine("};");

        // create the lambda instance
        string varName = nameDispo.GetName();
        LibReference.CreateLambdaInstance(lambdaClassName, varName, writer, mode switch { RuntimeRefMode.Argument => "runtime", _ => null});

        // load lambda captures
        foreach (LazyEvaluatable capture in lambda.ArgumentCaptures)
        {
            writer.WriteLine($"{varName}->{capture.Name} = {capture.Name};");
        }

        return varName;
    }

    private string ExpandLambdaCall(DynamicInvocation invocation, VarNameDistributor nameDispo, TextWriter writer, RuntimeRefMode mode)
    {
        List<string> arguments = [];
        foreach (IEvaluatable arg in invocation.Arguments)
        {
            arguments.Add(ProcessEvals(arg, nameDispo, writer, mode));
        }

        return $"{invocation.Prototype.Name}->Run({string.Join(", ", arguments.Prepend(mode switch { RuntimeRefMode.This => "this", RuntimeRefMode.Argument => "runtime", _ => throw new Exception("unexpected runtime reference mode") }))})";
    }

    private string ExpandValueGetter(TypeMemberGetter getter, VarNameDistributor nameDispo, TextWriter writer, RuntimeRefMode mode)
    {
        string subject = ProcessEvals(getter.Subject, nameDispo, writer, mode);
        return $"{subject}.{getter.Member}";
    }

    private string ExpandStructConstruction(StructuredData data, VarNameDistributor nameDispo, TextWriter writer, RuntimeRefMode mode)
    {
        List<string> members = [];
        foreach (IEvaluatable member in data.PositionalArguments)
        {
            members.Add(ProcessEvals(member, nameDispo, writer, mode));
        }

        string varname = nameDispo.GetName();
        writer.WriteLine($"{LibReference.PrintType(data.Type)} {varname} {{ {string.Join(", ", members)} }};");

        return varname;
    }

    private string ProcessEvals(IEvaluatable evaluatable, VarNameDistributor nameDispo, TextWriter implWriter, RuntimeRefMode mode) => evaluatable switch
    {
        LeafArithmetic leafArithmetic => ProcessEvals(leafArithmetic.Value, nameDispo, implWriter, mode),

        NodeArithmetic nodeArithmetic => ExpandNodeArithmetic(nodeArithmetic, nameDispo, implWriter, mode),

        Int32Value int32Value => int32Value.Value.ToString(),

        BooleanValue booleanValue => booleanValue.Value.ToString(),

        StaticInvocation call => ExpandFunctionCall(call, nameDispo, implWriter, mode),

        DynamicInvocation lambdaCall => ExpandLambdaCall(lambdaCall, nameDispo, implWriter, mode),

        LazyEvaluatable lazy => lazy.Name,

        ArrayLiteral arrLiteral => ExpandArrayCreation(arrLiteral, nameDispo, implWriter, mode),

        LambdaWithCaptures lambda => ExpandLambda(lambda, nameDispo, implWriter, mode),

        //TypeDefinition typeDef => $"struct {typeDef.Name} \n{{\n{string.Join("\n", typeDef.PositionalParameters.Select(name => $"    {typeDef.Parameters[name].Name} {name};"))}\n}};",

        StructuredData structData => ExpandStructConstruction(structData, nameDispo, implWriter, mode),

        TypeMemberGetter getter => ExpandValueGetter(getter, nameDispo, implWriter, mode),

        _ => throw new NotImplementedException($"the input semantic type is not supported for cpp code gen: {evaluatable.GetType().Name}")
    };

    public static IDisposable ConstructTypeDef(string name, TextWriter writer)
    {
        writer.WriteLine($"struct {name}");
        writer.WriteLine("{");

        return Disposable.Create(() =>
        {
            writer.WriteLine("};");
        });
    }

    public IDisposable ConstructMemeberFunctionDefinition(FunctionPrototype protoType, TextWriter writer)
    {
        writer.Write($"{LibReference.PrintType(protoType.ReturnType)} {LibReference.OutputClass}::{protoType.Name}({string.Join(", ", protoType.Params.Select(p => $"{LibReference.PrintType(p.Type)} {p.Name}"))})");

        writer.WriteLine("{");

        return Disposable.Create(() =>
        {
            writer.WriteLine("}");
        });
    }
}
