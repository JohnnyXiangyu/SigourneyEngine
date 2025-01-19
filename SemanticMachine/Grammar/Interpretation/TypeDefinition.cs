using System.Collections.Immutable;

namespace SemanticMachine.Grammar.Interpretation;

public record TypeDefinition(string Name, ImmutableDictionary<string, TypeDefinition> Members, ImmutableList<string> PositionalParameters) : ISemanticUnit
{
    public static TypeDefinition Resolve(string name, ImmutableDictionary<string, ISemanticUnit> symbolTable)
    {
        ISemanticUnit resolution = ISemanticUnit.ResolveSymbol(name, symbolTable);

        return resolution switch
        {
            TypeDefinition typeDef => typeDef,
            FunctionPrototype function => new CallableType(function.ReturnType, function.Params),
            _ => throw new Exception($"incorrect symbol reference for {name}, expecting type, got {resolution.GetType().Name}")
        };
    }

    public StructuredData Construct(IEvaluatable[] positionalArguments)
    {
        if (positionalArguments.Length != PositionalParameters.Count)
            throw new Exception($"constructor error, argument parameter length mismatch: {Name}");

        if (positionalArguments.Zip(PositionalParameters).Any(pair => pair.First.Type.Name != Members[pair.Second].Name))
            throw new Exception($"constructor error, argument type mismatch: {Name}");

        return new StructuredData(this, positionalArguments);
    }
}

public record StructuredData(TypeDefinition Prototype, IEvaluatable[] PositionalArguments) : IEvaluatable
{
    public TypeDefinition Type => Prototype;

    public IEnumerable<LazyEvaluatable> ArgumentCaptures => PositionalArguments.SelectMany(param => param.ArgumentCaptures);
}

public record ArrayType(TypeDefinition ElementType) : TypeDefinition(ElementType.Name + "[]", ImmutableDictionary<string, TypeDefinition>.Empty, []);

public record CallableType(TypeDefinition ReturnType, ImmutableList<LazyEvaluatable> Params) : TypeDefinition($"callable({ReturnType.Name}<{string.Join(',', Params.Select(x => x.Name))}>)", ImmutableDictionary<string, TypeDefinition>.Empty, [])
{
    private FunctionPrototype ToPrototype(string name) => new(name, false, ReturnType, Params);

    public DynamicInvocation Call(string functionAlias, IEvaluatable[] parameters) => new(ToPrototype(functionAlias), parameters);
}
