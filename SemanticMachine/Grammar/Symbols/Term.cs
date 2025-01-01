using ParserFramework;
using SemanticMachine.Grammar.Interpretation;
using System.Collections.Immutable;

namespace SemanticMachine.Grammar.Symbols;

public record Term() : INonTerminal
{
    public static ISymbol[][] Rules => [
        [new Literal()],
        [new NamedSymbol()],
        [new TerminalSymbol("("), new Expr(), new TerminalSymbol(")")],
        [new Invocation()],
        [new ValueGetter()]];

    public static IEvaluatable Verify(ParseTree[] children, ImmutableDictionary<string, ISemanticUnit> resolutionContext) =>
        children[0].Symbol switch
        {
            Literal => Literal.Verify(children[0].Children),
            NamedSymbol name => name.Verify<IEvaluatable>(resolutionContext),
            TerminalSymbol("(") => children[1] switch
            {
                ParseTree(Expr _, ParseTree[] exprChildren) => Expr.Verify(exprChildren, resolutionContext),
                _ => throw new Exception("unexpected code path")
            },
            Invocation => Invocation.Verify(children[0].Children, resolutionContext),
            ValueGetter => ValueGetter.Verify(children[0].Children, resolutionContext),
            _ => throw new Exception("parsing error, Term")
        };
}
