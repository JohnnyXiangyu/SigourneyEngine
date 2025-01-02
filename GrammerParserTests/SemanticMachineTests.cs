using ParserFramework;
using SemanticMachine.Grammar;
using SemanticMachine.Grammar.Interpretation;
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
        string code = "type Vector2 { int X, int Y, bool Z }";
        ParseTree? tree = GrammarRules.Parse(code, new TypeDecalaration());
        Assert.IsNotNull(tree);

        TypeDefinition typeDef = TypeDecalaration.Verify(tree.Children, IArithmetic.LoadArithmeticPrimitives(ImmutableDictionary<string, ISemanticUnit>.Empty));
        Console.WriteLine(typeDef.PrettyPrint());
        Assert.IsTrue(typeDef.Name == "Vector2");
        Assert.IsTrue(typeDef.Parameters["X"].Name == "int");
        Assert.IsTrue(typeDef.Parameters["Y"].Name == "int");
        Assert.IsTrue(typeDef.Parameters["Z"].Name == "bool");
    }

    //[TestMethod]
    //public void FunctionDefinitionVerificationTest()
    //{
    //    string define = "int AddTwice(int lhs, int rhs) => { lhs + rhs + rhs }";
    //    ParseTree? tree = GrammarRules.Parse(define, new FunctionDecalaration());
    //    Assert.IsNotNull(tree);

    //    FunctionDefinition declaration = FunctionDecalaration.VerifyFullDefinition(tree.Children, IArithmetic.LoadArithmeticPrimitives(ImmutableDictionary<string, ISemanticUnit>.Empty));
    //    Console.WriteLine(declaration.PrettyPrint());

    //    var context = ImmutableDictionary<string, ISemanticUnit>.Empty.Add(declaration.Prototype.Name, declaration);

    //    string invocation = "AddTwice(10, AddTwice(5, 1))";
    //    ParseTree? invocationTree = GrammarRules.Parse(invocation, new Invocation());
    //    Assert.IsNotNull(invocationTree);
    //    IEvaluatable call = Invocation.Verify(invocationTree.Children, context);

    //    Console.WriteLine(call.PrettyPrint());
    //}

    [TestMethod]
    public void ScriptParsingTest()
    {
        string code = "type Vector2 { int X, int Y, bool Z }; int AddTwice(int lhs, int rhs) => { lhs + rhs + rhs }";
        ParseTree? tree = GrammarRules.Parse(code, new Script());
        Assert.IsNotNull(tree);

        // load types
        ImmutableDictionary<string, ISemanticUnit> context = IArithmetic.LoadArithmeticPrimitives(ImmutableDictionary<string, ISemanticUnit>.Empty);
        var flattened = Script.Flatten(tree.Children);
        context = flattened.Aggregate(context, (oldContext, child) => child switch
        {
            ParseTree(TypeDecalaration, ParseTree[] decChildren) => TypeDecalaration.Verify(decChildren, oldContext) switch
            {
                TypeDefinition verifiedDef => oldContext.Add(verifiedDef.Name, verifiedDef)
            },
            _ => oldContext
        });

        // load function prototypes
        context = flattened.Aggregate(context, (oldContext, child) => child switch
        {
            ParseTree(FunctionDecalaration, ParseTree[] decChildren) => FunctionDecalaration.ExtractHeader(decChildren, oldContext) switch
            {
                FunctionPrototype prototype => oldContext.Add(prototype.Name, prototype)
            },
            _ => oldContext
        });

        // load actual function
        ImmutableDictionary<string, FunctionDefinition> functionDefinitions = flattened.Aggregate(ImmutableDictionary<string, FunctionDefinition>.Empty, (oldContext, child) => child switch
        {
            ParseTree(FunctionDecalaration, ParseTree[] decChildren) => FunctionDecalaration.VerifyFullDefinition(decChildren, context) switch
            {
                FunctionDefinition prototype => oldContext.Add(prototype.Prototype.Name, prototype)
            },
            _ => oldContext
        });

        // print everything out
        foreach (var pair in context)
        {
            Console.WriteLine(pair.Value.PrettyPrint());
        }
        foreach (var pair in functionDefinitions)
        {
            Console.WriteLine(pair.Value.PrettyPrint());
        }
    }
}
