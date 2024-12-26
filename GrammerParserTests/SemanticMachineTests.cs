using ParserFramework;
using SemanticMachine.Grammar;
using static SemanticMachine.Grammar.AllSymbols;

namespace GrammerParserTests;

[TestClass]
public class SemanticMachineTests
{
    private static void ParseSomething(string thing)
    {
        string[] tokens = GrammarRules.GetLexer().Lex(thing).Reverse().ToArray();
        IParseTree? parseTree = OmniParser.Parse(GrammarRules.GrammarFunc, new Expr(), tokens);

        Assert.IsNotNull(parseTree);
        Console.WriteLine(parseTree.PrettyPrint()?.ToString());
    }

    [TestMethod]
    public void ParseTest()
    {
        ParseSomething("results.Select(x => { x->Name }).Where(x, y, z => { x->Length = 1 })"); // whatever this is
        ParseSomething("a + b"); // a binop
        ParseSomething("x => { x + y + z}"); // a simple lambda
        ParseSomething("x, y => { x + y + z}"); // a random lambda
        ParseSomething("x, _ => { x }"); // a random lambda
    }
}
