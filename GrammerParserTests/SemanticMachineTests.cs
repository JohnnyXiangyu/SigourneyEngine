using ParserFramework;
using SemanticMachine.Grammar;
using SemanticMachine.Grammar.Interpretation;
using SemanticMachine.Grammar.Symbols;
using SemanticMachine.Grammar.Symbols.Decalaration;
using SemanticMachine.Grammar.Symbols.Expression;
using System.Collections.Immutable;

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

    
    public static void ArithmaticParseTest(string code)
    {
        string[] tokens = GrammarRules.GetLexer().Lex(code).Reverse().ToArray();
        ParseTree? tree = OmniParser.Parse(GrammarRules.GrammarFunc, new BinopChain(), tokens);

        Assert.IsNotNull(tree);

        // reverse the tree
        ParseTree postProcessedTree = GrammarRules.ReverseTreeBack(tree);

        IArithmetic arith = BinopChain.Verify(postProcessedTree.Children, ImmutableDictionary<string, ISemanticUnit>.Empty);
        Console.WriteLine(arith.PrettyPrint());
    }

    [TestMethod]
    public void ArithmaticParseTests()
    {
        ArithmaticParseTest("1 + 2 + 3");
        ArithmaticParseTest("1 + 2 * 3");
        ArithmaticParseTest("1 * 2 + 3");
        ArithmaticParseTest("1 * 2 * 3");
        ArithmaticParseTest("1 * 4 + 2 * 3");
        ArithmaticParseTest("1 - 2 - 3");

        ArithmaticParseTest("1 + 2 = 3");

        try
        {
            ArithmaticParseTest("1 + 2 && 3");
            throw new Exception("false negative");
        }
        catch { }

        ArithmaticParseTest("true && false");
        ArithmaticParseTest("true && true || false");
    }

    [TestMethod]
    public void PrintGrammar()
    {
        Console.WriteLine(GrammarRules.PrintGrammar());
    }

    [TestMethod]
    public void TypeDefinitionVerificationTest()
    {
        string code = "type Vector2 { int32 X, int32 Y, bool Z }";
        ParseTree? tree = GrammarRules.Parse(code, new TypeDecalaration());
        Assert.IsNotNull(tree);

        TypeDefinition typeDef = TypeDecalaration.Verify(tree.Children, IArithmetic.LoadArithmeticPrimitives(ImmutableDictionary<string, ISemanticUnit>.Empty));
        Assert.IsTrue(typeDef.Name == "Vector2");
        Assert.IsTrue(typeDef.Parameters["X"].Name == "int32");
        Assert.IsTrue(typeDef.Parameters["Y"].Name == "int32");
        Assert.IsTrue(typeDef.Parameters["Z"].Name == "bool");
    }

    [TestMethod]
    public void FunctionDefinitionVerificationTest()
    {
        string code = "int32 AddTwice(int32 lhs, int32 rhs) => { lhs + rhs + rhs }";
        ParseTree? tree = GrammarRules.Parse(code, new FunctionDecalaration());
        Assert.IsNotNull(tree);

        FunctionDefinition declaration = FunctionDecalaration.Verify(tree.Children, IArithmetic.LoadArithmeticPrimitives(ImmutableDictionary<string, ISemanticUnit>.Empty));

        Console.WriteLine("done?");
    }
}
