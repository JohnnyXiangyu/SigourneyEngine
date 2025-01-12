using SemanticMachine.Grammar.Interpretation;
using System.Collections.Immutable;
using System.Text;

namespace SemanticMachine.Binding;

public class CppCodeGenerator
{
    public CppLibraryReference LibReference { get; private set; } = new();

    public List<string> TypeDefintions { get; set; } = [];
    public List<string> FunctionPrototypes { get; set; } = [];
    public List<string> FunctionImplementations { get; set; } = [];
    public List<string> TemplateFunctions { get; set; } = [];

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

    private static string JoinLines(IEnumerable<string> lines) => string.Join(Environment.NewLine, lines.Where(l => l.Length > 0));

    public void GenerateFunction(FunctionDefinition definition)
    {
        VarNameDistributor dispo = new();

        string prototype = $"{LibReference.PrintType(definition.Prototype.ReturnType)} {definition.Prototype.Name}({string.Join(", ", definition.Prototype.Params.Select(p => $"{p.Type} {p.Name}"))});";

        FunctionPrototypes.Add(prototype);

        StringBuilder bodyBuilder = new();
        bodyBuilder
            .AppendLine($"{LibReference.PrintType(definition.Prototype.ReturnType)} {definition.Prototype.Name}({string.Join(", ", definition.Prototype.Params.Select(p => $"{p.Type} {p.Name}"))})")
            .AppendLine("{");

        (string prep, string value) = ProcessEvals(definition.Body, dispo);
        bodyBuilder.AppendLine(prep)
            .Append("return ")
            .Append(value)
            .AppendLine(";")
            .AppendLine("}");

        FunctionImplementations.Add(bodyBuilder.ToString());
    }

    private (string prep, string value) ExpandNodeArithmetic(NodeArithmetic node, VarNameDistributor nameDispo)
    {
        (string prepL, string valueL) = ProcessEvals(node.LeftChild, nameDispo);
        (string prepR, string valueR) = ProcessEvals(node.RightChild, nameDispo);

        string finalLine = $"{valueL} {SerializeOperation(node.Operation)} {valueR}";
        return (JoinLines([prepL, prepR]), finalLine);
    }

    private (string prep, string value) ExpandFunctionCall(FunctionCall call, VarNameDistributor nameDispo)
    {
        var pair = call.Arguments.Aggregate((ImmutableList<string>.Empty, ImmutableList<string>.Empty), (seed, newEvaluatable) => ProcessEvals(newEvaluatable, nameDispo) switch
        {
            (string prep, string name) => (seed.Item1.Add(prep), seed.Item2.Add(name)),
        });

        string finalLine = $"{call.Function.Name}({string.Join(", ", pair.Item2)});";

        return (JoinLines(pair.Item1), finalLine);
    }

    private (string prep, string value) ExpandArrayCreation(ArrayLiteral initializer, VarNameDistributor nameDispo)
    {
        (ImmutableList<string> allPrep, ImmutableList<string> allNames) = initializer.Elements.Aggregate((ImmutableList<string>.Empty, ImmutableList<string>.Empty), (pair, value) =>
        {
            (string prep, string name) = ProcessEvals(value, nameDispo);
            return (pair.Item1.Add(prep), pair.Item2.Add(name));
        });

        return LibReference.CreateArrayEnumerable(LibReference.PrintType(initializer.ElementType), [.. allNames], nameDispo) switch
        {
            (string prep, string name) => (JoinLines([.. allPrep, prep]), name)
        };
    }

    private (string prep, string value) ProcessEvals(IEvaluatable evaluatable, VarNameDistributor nameDispo) => evaluatable switch
    {
        LeafArithmetic leafArithmetic => ProcessEvals(leafArithmetic.Value, nameDispo),

        NodeArithmetic nodeArithmetic => ExpandNodeArithmetic(nodeArithmetic, nameDispo),
        
        Int32Value int32Value => (string.Empty, int32Value.Value.ToString()),
        
        BooleanValue booleanValue => (string.Empty, booleanValue.Value.ToString()),
        
        FunctionCall call => ExpandFunctionCall(call, nameDispo),
        
        LazyEvaluatable lazy => (string.Empty, lazy.Name),

        ArrayLiteral arrLiteral => ExpandArrayCreation(arrLiteral, nameDispo),
        
        //PrimitiveType => (string.Empty, string.Empty),
        
        //TypeDefinition typeDef => $"struct {typeDef.Name} \n{{\n{string.Join("\n", typeDef.PositionalParameters.Select(name => $"    {typeDef.Parameters[name].Name} {name};"))}\n}};",
        
        //StructuredData structData => $"{structData.Type}({string.Join(", ", structData.PositionalArguments.Select(ProduceCppCode))})",
        
        //TypeMemberGetter getter => $"{ProduceCppCode(getter.Subject)}.{getter.Member}",

        _ => throw new NotImplementedException($"the input semantic type is not supported for evaluatable: {evaluatable.GetType().Name}")
    };
}
