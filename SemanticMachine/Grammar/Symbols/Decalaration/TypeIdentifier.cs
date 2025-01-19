using ParserFramework;
using SemanticMachine.Grammar.Interpretation;
using System.Collections.Immutable;

namespace SemanticMachine.Grammar.Symbols.Decalaration;
public record TypeIdentifier : INonTerminal
{
    public static ISymbol[][] Rules => [
        [new NamedSymbol()],
        [new TypeIdentifier(), new TerminalSymbol("[]")],
        [new LambdaType()]];

    public static TypeDefinition Resolve(ParseTree[] children, ImmutableDictionary<string, ISemanticUnit> symbolTable) => children switch
    {
        [ParseTree(TypeIdentifier, ParseTree[] baseTypeChildren), ParseTree(TerminalSymbol("[]"), [])] => new ArrayType(Resolve(baseTypeChildren, symbolTable)),
        [ParseTree(NamedSymbol(string name), [])] => TypeDefinition.Resolve(name, symbolTable),
        [ParseTree(LambdaType, ParseTree[] lambdaTypeChildren)] => LambdaType.Resolve(lambdaTypeChildren, symbolTable),
        _ => throw new Exception($"parsing error, {nameof(TypeIdentifier)}")
    };
}

public record TypeList : INonTerminal
{
    public static ISymbol[][] Rules => [
        [new TypeIdentifier()],
        [new TypeList(), new TerminalSymbol(","), new TypeIdentifier()]];

    public static ImmutableList<TypeDefinition> Flatten(ParseTree[] children, ImmutableDictionary<string, ISemanticUnit> symbolTable) => children switch
    {
        [ParseTree(TypeIdentifier, ParseTree[] idChildren)] => [TypeIdentifier.Resolve(idChildren, symbolTable)],
        [ParseTree(TypeList, ParseTree[] listChildren), _, ParseTree(TypeIdentifier, ParseTree[] idChildren)] => [.. Flatten(listChildren, symbolTable), TypeIdentifier.Resolve(idChildren, symbolTable)],
        _ => throw new Exception("parsing error, TypeList")
    };
}

public record LambdaType : INonTerminal
{
    public static ISymbol[][] Rules => [
        [new TerminalSymbol("func"), new TerminalSymbol("<"), new TypeList(), new TerminalSymbol(">")]];

    public static TypeDefinition Resolve(ParseTree[] children, ImmutableDictionary<string, ISemanticUnit> symbolTable) => children switch
    {
        [_, _, ParseTree(TypeList, ParseTree[] listChildren), _] => TypeList.Flatten(listChildren, symbolTable).ToArray() switch
        {
            [TypeDefinition returnType, .. TypeDefinition[] parameters] => new CallableType(returnType, parameters.Select(t => new LazyEvaluatable(t, string.Empty)).ToImmutableList()),
            _ => throw new Exception("lambda functions must return a value")
        },
        _ => throw new Exception("parsing error, LambdaType")
    };
}
