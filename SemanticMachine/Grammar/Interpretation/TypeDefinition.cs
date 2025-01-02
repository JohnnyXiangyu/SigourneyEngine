using System.Collections.Immutable;

namespace SemanticMachine.Grammar.Interpretation;

public record TypeDefinition(string Name, ImmutableDictionary<string, TypeDefinition> Parameters, ImmutableList<string> PositionalParameters) : ISemanticUnit
{
    public static TypeDefinition Resolve(string name, ImmutableDictionary<string, ISemanticUnit> context)
    {
        ISemanticUnit resolution = ISemanticUnit.ResolveSymbol(name, context);

        if (resolution is not TypeDefinition typeDef)
            throw new Exception($"incorrect symbol reference for {name}, expecting type, got {resolution.GetType().Name}");

        return typeDef;
    }

    public StructuredData Construct(IEvaluatable[] positionalArguments)
    {
        if (positionalArguments.Length != PositionalParameters.Count)
            throw new Exception($"constructor error, argument parameter length mismatch: {Name}");

        if (positionalArguments.Zip(PositionalParameters).Any(pair => pair.First.Type != Parameters[pair.Second].Name))
            throw new Exception($"constructor error, argument type mismatch: {Name}");

        return new StructuredData(this, positionalArguments);
    }

    public string PrettyPrint() => $"struct {Name} \n{{\n{string.Join("\n", PositionalParameters.Select(name => $"    {Parameters[name].Name} {name};"))}\n}};\n";
}

public record StructuredData(TypeDefinition Prototype, IEvaluatable[] PositionalArguments) : IEvaluatable
{
    public string Type => Prototype.Name;

    public string PrettyPrint() => $"{Type}({string.Join(", ", PositionalArguments.Select(e => e.PrettyPrint()))})";
}

public record PrimitiveType(string Name) : TypeDefinition(Name, ImmutableDictionary<string, TypeDefinition>.Empty, []);
