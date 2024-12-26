using System.Data;
using ParserFramework;
using System.Text.Json;

namespace GrammerParserTests;

using Rule = ISymbol[];
using RuleSet = ISymbol[][];
using Grammar = Func<ISymbol, ISymbol[][]>;
using Acceptor = Func<AcceptableState, AcceptableState?>;

[TestClass]
public class ParserFrameworkTest
{
    private record Expr : INonTerminal;
    private record Term : INonTerminal;

    private record Binop : INonTerminal;
    
    private record Num : ITerminal
    {
        public bool Accept(string token)
        {
            try
            {
                int.Parse(token);
                return true;
            }
            catch
            {
                return false;
            }
        }
    }

    private static ISymbol[][] Grammar(ISymbol target) => target switch
    {
        Expr => [
            [new Term(), new Binop(), new Expr()],
                [new Term()]],
        Term => [
            [new Num()],
                [new TerminalSymbol("("), new Expr(), new TerminalSymbol(")")]],
        Binop => [
            [new TerminalSymbol("+")],
                [new TerminalSymbol("-")]],
        _ => throw new Exception("unexpected target symbol type in test")
    };

    [TestMethod]
    public void TestOnlyParse()
    {
        IParseTree? tree = OmniParser.Parse(Grammar, new Expr(), "1+(2-1)+3+1".Select(x => x.ToString()).ToArray());

        Assert.IsNotNull(tree);

        Console.WriteLine(tree.PrettyPrint()?.ToString());
    }
}
