using ParserFramework;
using SemanticMachine.Grammar.Interpretation;
using System.Collections.Immutable;

namespace SemanticMachine.Grammar.Symbols.Expression;

public record ValueGetter() : INonTerminal
{
    public static ISymbol[][] Rules => [[new Term(), new TerminalSymbol("->"), new NamedSymbol()]];

    public static IEvaluatable Verify(ParseTree[] children, ImmutableDictionary<string, ISemanticUnit> context) =>
        children switch
        {
        [ParseTree(Term _, ParseTree[] termChildren), _, ParseTree(NamedSymbol(string memberName), [])] => new TypeMemberGetter(Term.Verify(termChildren, context), memberName, context),
            _ => throw new Exception("parsing error, ValueGetter")
        };
}
