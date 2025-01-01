using ParserFramework;
using System.Data;

namespace GrammerParserTests;

[TestClass]
public class ParserFrameworkTest
{
    private record Expr : INonTerminal;
    private record Term : INonTerminal;

    private record Binop : INonTerminal;
    
    private record Num(string Value = "") : ITerminal
    {
        public ITerminal? Accept(string token)
        {
            try
            {
                int.Parse(token);
                return new Num(token);
            }
            catch
            {
                return null;
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
        ParseTree? tree = OmniParser.Parse(Grammar, new Expr(), "1+(2-1)+3+1".Select(x => x.ToString()).ToArray());

        Assert.IsNotNull(tree);

        Console.WriteLine(tree.PrettyPrint()?.ToString());
    }

    [TestMethod]
    public void TokenizerTest()
    {
        string input = "1 + 2 + 3    + \n(1*4 = 3) + 10 == 10";
        string[] target = ["1", "+", "2", "+", "3", "+", "(", "1", "*", "4", "=", "3", ")", "+", "10", "==", "10"];
        var lexer = new WaterfallLexer(["==", "+", "*", "(", ")", "="], [" ", "\n"]);
        string[] result = lexer.Lex(input);

        Assert.IsTrue(target.Length == result.Length);
        Assert.IsTrue(target.Zip(result).Select(pair => pair.First.Equals(pair.Second)).All(x => x));
    }
}
