namespace ParserFramework;

using Rule = ISymbol[];
using RuleSet = ISymbol[][];
using Grammar = Func<ISymbol, ISymbol[][]>;
using Acceptor = Func<AcceptableState, AcceptableState?>;

public record AcceptableState(string[] Suffix, IParseTree Progress);

public class OmniParser
{
    public static AcceptableState? ParseSymbol(Grammar grammar, ISymbol symbol, Acceptor acceptor, AcceptableState state) =>
        state.Suffix switch
        {
            [] => null,
            [string head, .. string[] tail] => symbol switch
            {
                INonTerminal nonTerm => ParseLevel(grammar, grammar(nonTerm), acceptor, state with { Progress = new Node(nonTerm, []) }),
                ITerminal term => term.Accept(head) ? acceptor(new AcceptableState(tail, new Leaf(term))) : null,
                _ => throw new Exception("unexpected pattern match branch")
            }
        };

    public static AcceptableState? ParseAlternative(Grammar grammar, IParseTree oldProgress, Rule remainingRule, Acceptor acceptor, AcceptableState state)
    {
        IParseTree newProgress = state.Progress switch
        {
            Node(ISymbol _, []) => oldProgress,
            _ => oldProgress switch
            {
                Node(ISymbol parentOld, IParseTree[] childrenOld) => new Node(parentOld, [..childrenOld, state.Progress]),
                _ => throw new Exception("this pattern is just to eliminate the error, should never be used")
            }
        };

        return remainingRule switch
        {
            [] => acceptor(state with { Progress = newProgress }),
            [ISymbol head, .. Rule tail] => ParseSymbol(grammar, head, (AcceptableState inState) => ParseAlternative(grammar, newProgress, tail, acceptor, inState), state)
        };
    }


    public static AcceptableState? ParseLevel(Grammar grammar, RuleSet rules, Acceptor acceptor, AcceptableState state) =>
        rules switch
        {
            [] => null,
            [Rule rule, .. RuleSet tail] => ParseAlternative(grammar, state.Progress, rule, acceptor, state) switch
            {
                AcceptableState newState => newState,
                null => ParseLevel(grammar, tail, acceptor, state)
            }
        };

    public static AcceptableState? EmptyAcceptor(AcceptableState state) =>
        state.Suffix switch
        {
            [] => state,
            _ => null
        };

    public static IParseTree? Parse(Grammar grammar, ISymbol rootSymbol, string[] fragment, Acceptor? initialAcceptor = null) =>
        ParseLevel(grammar, grammar(rootSymbol), initialAcceptor ?? EmptyAcceptor, new AcceptableState(fragment, new Node(rootSymbol, [])))?.Progress;
}
