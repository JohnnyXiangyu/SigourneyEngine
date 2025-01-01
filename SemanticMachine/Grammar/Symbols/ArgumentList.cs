using ParserFramework;
using SemanticMachine.Grammar.Interpretation;
using SemanticMachine.Grammar.Symbols;
using System.Collections.Immutable;

namespace SemanticMachine.Grammar;

public record ArgumentList() : INonTerminal
{
    public static ISymbol[][] Rules => [
        [new ArgumentList(), new TerminalSymbol(","), new Expr()],
        [new Expr()],
        [new TerminalSymbol("_")]];

    public static IEvaluatable[] Verify(ParseTree[] children, ImmutableDictionary<string, ISemanticUnit> resolutionContext) =>
        children switch
        {
            [ParseTree(ArgumentList _, ParseTree[] otherChildren), _, ParseTree(Expr _, ParseTree[] exprChildren)] => [.. Verify(otherChildren, resolutionContext), Expr.Verify(exprChildren, resolutionContext)],
            [ParseTree(Expr _, ParseTree[] expChildren)] => [Expr.Verify(expChildren, resolutionContext)],
            [ParseTree(TerminalSymbol("_"), [])] => [],
            _ => throw new Exception("parsing error, ArgumentList")
        };
}
