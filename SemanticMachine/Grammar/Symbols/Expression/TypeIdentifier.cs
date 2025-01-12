using ParserFramework;
using SemanticMachine.Grammar.Interpretation;
using System.Collections.Immutable;

namespace SemanticMachine.Grammar.Symbols.Expression;
public record TypeIdentifier : INonTerminal
{
    public static ISymbol[][] Rules => [
        [new NamedSymbol()],
        [new TypeIdentifier(), new TerminalSymbol("[]")]];

    public static TypeDefinition Resolve(ParseTree[] children, ImmutableDictionary<string, ISemanticUnit> symbolTable) => children switch
    {
        [ParseTree(TypeIdentifier, ParseTree[] baseTypeChildren), ParseTree(TerminalSymbol("[]"), [])] => new ArrayType(Resolve(baseTypeChildren, symbolTable)),
        [ParseTree(NamedSymbol(string name), [])] => TypeDefinition.Resolve(name, symbolTable),
        _ => throw new Exception($"parsing error, {nameof(TypeIdentifier)}")
    };
}
