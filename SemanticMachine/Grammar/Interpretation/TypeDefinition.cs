using System.Collections.Immutable;

namespace SemanticMachine.Grammar.Interpretation;

public record TypeDefinition(string Name, ImmutableDictionary<string, TypeDefinition> Parameters) : ISemanticUnit
{
    public static TypeDefinition Resolve(string name, ImmutableDictionary<string, ISemanticUnit> context)
    {
        ISemanticUnit resolution = ISemanticUnit.ResolveSymbol(name, context);

        if (resolution is not TypeDefinition typeDef)
            throw new Exception($"incorrect symbol reference for {name}, expecting type, got {resolution.GetType().Name}");

        return typeDef;
    }
}

public record PrimitiveType(string Name) : TypeDefinition(Name, ImmutableDictionary<string, TypeDefinition>.Empty);
