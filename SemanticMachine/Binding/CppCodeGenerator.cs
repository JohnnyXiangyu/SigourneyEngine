﻿using Nito.Disposables;
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

        string value = ProcessEvals(definition.Body, dispo, impl);

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

    private string ExpandNodeArithmetic(NodeArithmetic node, VarNameDistributor nameDispo, TextWriter writer)
    {
        string valueL = ProcessEvals(node.LeftChild, nameDispo, writer);
        string valueR = ProcessEvals(node.RightChild, nameDispo, writer);

        return $"{valueL} {SerializeOperation(node.Operation)} {valueR}";
    }

    private string ExpandFunctionCall(StaticInvocation call, VarNameDistributor nameDispo, TextWriter writer)
    {
        List<string> parameters = [];
        foreach (IEvaluatable parameter in call.Arguments)
        {
            parameters.Add(ProcessEvals(parameter, nameDispo, writer));
        }

        return $"{call.Function.Name}({string.Join(", ", parameters)})";
    }

    private string ExpandArrayCreation(ArrayLiteral initializer, VarNameDistributor nameDispo, TextWriter writer)
    {
        List<string> elements = [];
        foreach (IEvaluatable element in initializer.Elements)
        {
            elements.Add(ProcessEvals(element, nameDispo, writer));
        }

        return LibReference.CreateArrayEnumerable(LibReference.PrintType(initializer.ElementType), [.. elements], nameDispo, writer);
    }

    private string ExpandLambda(LambdaWithCaptures lambda, VarNameDistributor nameDispo, TextWriter writer)
    {
        // use the lambda constructor to create a new struct (all captures by value)
        IEnumerable<string> captures = lambda.ArgumentCaptures.Select(cap => cap.Name).Append("this");
        IEnumerable<string> parameterTypes = lambda.Params.Select(par => LibReference.PrintType(par.ParamType));
        IEnumerable<string> parameters = lambda.Params.Select(par => $"{LibReference.PrintType(par.ParamType)} {par.Name}");
        string returnType = LibReference.PrintType(lambda.Body.Type);

        string lambdaType = LibReference.PrintType(lambda.Type);
        string lambdaName = nameDispo.GetName();

        writer.Write($"{lambdaType} {lambdaName} = ");
        writer.Write($"{LibReference.MainNamespace}::{LibReference.FunctionalNamespace}::LambdaConstructor<{string.Join(", ", parameterTypes.Prepend(returnType))}>::Construct(GetAllocator(), ");
        writer.WriteLine($"[{string.Join(", ", captures)}]({string.Join(", ", parameters)})");
        writer.WriteLine("{");

        string innerResult = ProcessEvals(lambda.Body, nameDispo, writer);
        writer.WriteLine($"return {innerResult};");
        writer.WriteLine("});");

        return lambdaName;
    }

    private string ExpandLambdaCall(DynamicInvocation invocation, VarNameDistributor nameDispo, TextWriter writer)
    {
        List<string> arguments = [];
        foreach (IEvaluatable arg in invocation.Arguments)
        {
            arguments.Add(ProcessEvals(arg, nameDispo, writer));
        }

        return $"{invocation.Prototype.Name}->Run({string.Join(", ", arguments)})";
    }

    private string ExpandValueGetter(TypeMemberGetter getter, VarNameDistributor nameDispo, TextWriter writer)
    {
        string subject = ProcessEvals(getter.Subject, nameDispo, writer);
        return $"{subject}.{getter.Member}";
    }

    private string ExpandStructConstruction(StructuredData data, VarNameDistributor nameDispo, TextWriter writer)
    {
        List<string> members = [];
        foreach (IEvaluatable member in data.PositionalArguments)
        {
            members.Add(ProcessEvals(member, nameDispo, writer));
        }

        string varname = nameDispo.GetName();
        writer.WriteLine($"{LibReference.PrintType(data.Type)} {varname} {{ {string.Join(", ", members)} }};");

        return varname;
    }

    private string ProcessEvals(IEvaluatable evaluatable, VarNameDistributor nameDispo, TextWriter implWriter) => evaluatable switch
    {
        LeafArithmetic leafArithmetic => ProcessEvals(leafArithmetic.Value, nameDispo, implWriter),

        NodeArithmetic nodeArithmetic => ExpandNodeArithmetic(nodeArithmetic, nameDispo, implWriter),

        Int32Value int32Value => int32Value.Value.ToString(),

        BooleanValue booleanValue => booleanValue.Value.ToString(),

        StaticInvocation call => ExpandFunctionCall(call, nameDispo, implWriter),

        DynamicInvocation lambdaCall => ExpandLambdaCall(lambdaCall, nameDispo, implWriter),

        LazyEvaluatable lazy => lazy.Name,

        ArrayLiteral arrLiteral => ExpandArrayCreation(arrLiteral, nameDispo, implWriter),

        LambdaWithCaptures lambda => ExpandLambda(lambda, nameDispo, implWriter),

        //TypeDefinition typeDef => $"struct {typeDef.Name} \n{{\n{string.Join("\n", typeDef.PositionalParameters.Select(name => $"    {typeDef.Parameters[name].Name} {name};"))}\n}};",

        StructuredData structData => ExpandStructConstruction(structData, nameDispo, implWriter),

        TypeMemberGetter getter => ExpandValueGetter(getter, nameDispo, implWriter),

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
