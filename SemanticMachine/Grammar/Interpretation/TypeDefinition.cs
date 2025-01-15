using System.Collections.Immutable;

namespace SemanticMachine.Grammar.Interpretation;

public record TypeDefinition(string Name, ImmutableDictionary<string, TypeDefinition> Parameters, ImmutableList<string> PositionalParameters) : ISemanticUnit
{
    public static TypeDefinition Resolve(string name, ImmutableDictionary<string, ISemanticUnit> symbolTable)
    {
        ISemanticUnit resolution = ISemanticUnit.ResolveSymbol(name, symbolTable);

        if (resolution is not TypeDefinition typeDef)
            throw new Exception($"incorrect symbol reference for {name}, expecting type, got {resolution.GetType().Name}");

        return typeDef;
    }

    public StructuredData Construct(IEvaluatable[] positionalArguments)
    {
        if (positionalArguments.Length != PositionalParameters.Count)
            throw new Exception($"constructor error, argument parameter length mismatch: {Name}");

        if (positionalArguments.Zip(PositionalParameters).Any(pair => pair.First.Type.Name != Parameters[pair.Second].Name))
            throw new Exception($"constructor error, argument type mismatch: {Name}");

        return new StructuredData(this, positionalArguments);
    }
}

public record StructuredData(TypeDefinition Prototype, IEvaluatable[] PositionalArguments) : IEvaluatable
{
    public TypeDefinition Type => Prototype;

    public IEnumerable<string> ParamReferences => PositionalArguments.SelectMany(param => param.ParamReferences);
}

public record ArrayType(TypeDefinition ElementType) : TypeDefinition(ElementType.Name + "[]", ImmutableDictionary<string, TypeDefinition>.Empty, []);
