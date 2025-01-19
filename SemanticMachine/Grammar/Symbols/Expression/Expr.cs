using ParserFramework;
using SemanticMachine.Grammar.Interpretation;
using System.Collections.Immutable;

namespace SemanticMachine.Grammar.Symbols.Expression;
public class Expr() : INonTerminal
{
    public static ISymbol[][] Rules => [
        [new BinopChain()],
        [new Term()],
        [new ValueGetter()],
        [new Lambda()]];

    public static IEvaluatable Verify(ParseTree[] children, ImmutableDictionary<string, ISemanticUnit> context) => children switch
    {
        [ParseTree(BinopChain, ParseTree[] binopChildren)] => BinopChain.Verify(binopChildren, context),
        [ParseTree(Term _, ParseTree[] termChildren)] => Term.Verify(termChildren, context),
        [ParseTree(ValueGetter, ParseTree[] getterChildren)] => ValueGetter.Verify(getterChildren, context),
        [ParseTree(Lambda, ParseTree[] lambdaChildren)] => Lambda.Verify(lambdaChildren, context),
        _ => throw new Exception("parser error, Expr")
    };
}
