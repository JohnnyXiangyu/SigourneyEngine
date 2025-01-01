using ParserFramework;
using SemanticMachine.Grammar.Interpretation;
using System.Collections.Immutable;

namespace SemanticMachine.Grammar.Symbols;

public record Binop(string Token = "") : ITerminal
{
    private static readonly HashSet<string> s_BinOps = "+-*/=&|".Select(c => c.ToString()).Concat(["&&", "||"]).ToHashSet();

    public ITerminal? Accept(string token) => s_BinOps.Contains(token) ? this with { Token = token } : null;
}

public record BinopChain() : INonTerminal
{
    public static ISymbol[][] Rules => [
        [new Term(), new Binop(), new Term()],
        [new BinopChain(), new Binop(), new Term()]];

    private static ParseTree[] Flatten(ParseTree[] children, ImmutableDictionary<string, ISemanticUnit> context) =>
        children switch
        {
            [ParseTree(BinopChain _, ParseTree[] chainChildren), _, _] => [.. Flatten(chainChildren, context), .. children.Skip(1)],
            _ => children
        };

    private static List<ParseTree> Rearrange(List<ParseTree> flatTree, ImmutableDictionary<string, ISemanticUnit> context, List<ParseTree> prefix) =>
        flatTree switch
        {
        [] => throw new Exception("unexpected input: source list should never be empty, BinopChain.Rearrange"),
        [ParseTree lastTerm] => [.. prefix, lastTerm],
        [ParseTree headTerm, ParseTree headOp, .. List<ParseTree> tail] => prefix switch
        {
        [] => Rearrange(tail, context, [headOp, headTerm]),
        [ParseTree(Binop(string operation), _), .. _] =>
            IArithmatic.ArithmaticOpAltitude(((Binop)headOp.Symbol).Token) > IArithmatic.ArithmaticOpAltitude(operation) ?
                Rearrange(tail, context, [.. prefix, headOp, headTerm])
                :
                Rearrange(tail, context, [headOp, .. prefix, headTerm]),
            _ => throw new Exception("prefix is not headed by an binary operator")
        }
        };

    private static (IArithmatic Tree, List<ParseTree> Suffix) TreeConstruction(List<ParseTree> rawTree, ImmutableDictionary<string, ISemanticUnit> context) => 
        rawTree switch
        {
            [ParseTree(Binop(string operation), _), .. List<ParseTree> tail] => TreeConstruction(tail, context) switch
            {
                (IArithmatic subTreeOne, List<ParseTree> newSuffix) => TreeConstruction(newSuffix, context) switch
                {
                    (IArithmatic subTreeTwo, List<ParseTree> finalSuffix) => (new NodeArithmatic(subTreeOne, subTreeTwo, IArithmatic.DeserializeOperation(operation)), finalSuffix)
                }
            },
            [ParseTree(Term _, ParseTree[] termChildren), .. List<ParseTree> tail] => (new LeafArithmatic(Term.Verify(termChildren, context)), tail),
            [] => throw new Exception("input should not be empty, BinopChain.TreeConstruction"),
            _ => throw new Exception("unexpected input, BinopChain.TreeConstruction")
        };

    public static IArithmatic Verify(ParseTree[] children, ImmutableDictionary<string, ISemanticUnit> context) =>
        TreeConstruction(Rearrange([.. Flatten(children, context)], context, []), context).Tree;
}

