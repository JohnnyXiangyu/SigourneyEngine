using ParserFramework;
using SemanticMachine.Grammar;
using SemanticMachine.Grammar.Symbols;

namespace GrammerParserTests;

[TestClass]
public class SemanticMachineTests
{
    private static void ParseSomething(string thing)
    {
        string[] tokens = GrammarRules.GetLexer().Lex(thing).Reverse().ToArray();
        ParseTree? parseTree = OmniParser.Parse(GrammarRules.GrammarFunc, new Expr(), tokens);

        Assert.IsNotNull(parseTree);
        Console.WriteLine(parseTree.PrettyPrint()?.ToString());
    }

    [TestMethod]
    public void ParseTest()
    {
        // TODO: need to add back support for lambda
        //ParseSomething("results.Select(x => { x->Name }).Where(x, y, z => { x->Length = 1 })"); // whatever this is
        ParseSomething("a + b"); // a binop
        ParseSomething("a + b * c * d"); // a binop
        //ParseSomething("x => { x + y + z}"); // a simple lambda
        //ParseSomething("x, y => { x + y + z}"); // a random lambda
        //ParseSomething("x, _ => { x }"); // a random lambda
    }

    [TestMethod]
    public void ArithmaticParseTest()
    {
        string code = "1 + 2 + 3"; // a binop chain
        string[] tokens = GrammarRules.GetLexer().Lex(code).Reverse().ToArray();
        ParseTree? tree = OmniParser.Parse(GrammarRules.GrammarFunc, new BinopChain(), tokens);
    }
}
