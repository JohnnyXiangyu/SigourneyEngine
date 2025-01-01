namespace ParserFramework;

using Rule = ISymbol[];
using RuleSet = ISymbol[][];
using Grammar = Func<ISymbol, ISymbol[][]>;
using Acceptor = Func<AcceptableState, AcceptableState?>;

public record AcceptableState(string[] Suffix, ParseTree Progress);

public class OmniParser
{
    public static AcceptableState? ParseSymbol(Grammar grammar, ISymbol symbol, Acceptor acceptor, AcceptableState state) =>
        state.Suffix switch
        {
            [] => null,
            [string head, .. string[] tail] => symbol switch
            {
                INonTerminal nonTerm => ParseLevel(grammar, grammar(nonTerm), acceptor, state with { Progress = new TreeNode(nonTerm, []) }),
                ITerminal term => term.Accept(head) switch
                {
                    null => null,
                    ITerminal accepted => acceptor(new AcceptableState(tail, new TreeLeaf(accepted)))
                },
                _ => throw new Exception("unexpected pattern match branch")
            }
        };

    public static AcceptableState? ParseAlternative(Grammar grammar, ParseTree oldProgress, Rule remainingRule, Acceptor acceptor, AcceptableState state)
    {
        ParseTree newProgress = state.Progress switch
        {
            TreeNode(ISymbol _, []) => oldProgress,
            _ => oldProgress switch
            {
                TreeNode(ISymbol parentOld, ParseTree[] childrenOld) => new TreeNode(parentOld, [..childrenOld, state.Progress]),
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

    public static ParseTree? Parse(Grammar grammar, ISymbol rootSymbol, string[] fragment, Acceptor? initialAcceptor = null) =>
        ParseLevel(grammar, grammar(rootSymbol), initialAcceptor ?? EmptyAcceptor, new AcceptableState(fragment, new TreeNode(rootSymbol, [])))?.Progress;
}
