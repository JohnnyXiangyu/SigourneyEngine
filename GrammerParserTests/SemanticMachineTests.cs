using ParserFramework;
using SemanticMachine.Binding;
using SemanticMachine.Grammar;
using SemanticMachine.Grammar.Interpretation;
using SemanticMachine.Grammar.Symbols.Decalaration;
using SemanticMachine.Grammar.Symbols.Expression;
using System.Collections.Immutable;

namespace GrammerParserTests;

[TestClass]
public class SemanticMachineTests
{
    [TestMethod]
    public void PrintGrammar()
    {
        Console.WriteLine(GrammarRules.PrintGrammar());
    }

    private static void PrintFunctionCode(ParseTree tree)
    {
        using StreamWriter headerWriter = new(new MemoryStream());
        using StreamWriter implWriter = new(new MemoryStream());

        CppCodeGenerator binder = new();

        // load types
        ImmutableDictionary<string, ISemanticUnit> context = IArithmetic.LoadArithmeticPrimitives(ImmutableDictionary<string, ISemanticUnit>.Empty);
        var flattened = Script.Flatten(tree.Children);

        var pair = flattened.Aggregate((context, ImmutableList<TypeDefinition>.Empty), (oldContext, child) => child switch
        {
            ParseTree(TypeDecalaration, ParseTree[] decChildren) => TypeDecalaration.Verify(decChildren, oldContext.context) switch
            {
                TypeDefinition verifiedDef => (oldContext.context.Add(verifiedDef.Name, verifiedDef), oldContext.Empty.Add(verifiedDef))
            },
            _ => oldContext
        });

        context = pair.context;

        foreach (TypeDefinition def in pair.Empty)
        {
            binder.GenerateTypeDef(def, headerWriter);
        }

        // load function prototypes
        context = flattened.Aggregate(context, (oldContext, child) => child switch
        {
            ParseTree(MaybeInlineFunction, ParseTree[] decChildren) => MaybeInlineFunction.ExtractHeader(decChildren, oldContext) switch
            {
                FunctionPrototype prototype => oldContext.Add(prototype.Name, prototype)
            },
            _ => oldContext
        });

        // load actual function
        ImmutableDictionary<string, FunctionDefinition> functionDefinitions = flattened.Aggregate(ImmutableDictionary<string, FunctionDefinition>.Empty, (oldContext, child) => child switch
        {
            ParseTree(MaybeInlineFunction, ParseTree[] decChildren) => MaybeInlineFunction.VerifyFullDefinition(decChildren, context) switch
            {
                FunctionDefinition prototype => oldContext.Add(prototype.Prototype.Name, prototype)
            },
            _ => oldContext
        });

        foreach (var functionDef in functionDefinitions.Values)
        {
            binder.GenerateFunction(functionDef, headerWriter, implWriter);
        }
    }

    [TestMethod]
    public void ArrayFunctionTest()
    {
        string code = "int[][] Foobar() => [[1, 2], [3]];";
        ParseTree? tree = GrammarRules.Parse(code, new Script());
        Assert.IsNotNull(tree);

        PrintFunctionCode(tree);
    }

    [TestMethod]
    public void AutomaticCaptuerTest()
    {
        string code = "int foobar(int a, int b) => a  + 3 * 10";
        ParseTree? tree = GrammarRules.Parse(code, new FunctionDeclaration());
        Assert.IsNotNull(tree);

        ImmutableDictionary<string, ISemanticUnit> context =
            IArithmetic.LoadArithmeticPrimitives(ImmutableDictionary<string, ISemanticUnit>.Empty);
        context = context.Add("foobar", FunctionDeclaration.ExtractHeader(tree.Children, context));
        FunctionDefinition expression = FunctionDeclaration.VerifyFullDefinition(tree.Children, context);
        foreach (var capture in expression.Body.ArgumentCaptures)
        {
            Console.WriteLine($"{capture.Name} : {capture.Type.Name}");
        }
    }

    [TestMethod]
    public void LambdaCreationTest()
    {
        string code = "func<int, int> Plus(int a) => (int b) => (a + b);";
        ParseTree? tree = GrammarRules.Parse(code, new Script());
        Assert.IsNotNull(tree);

        PrintFunctionCode(tree);
    }

    [TestMethod]
    public void FunctionWithLambdaTest()
    {
        string code = "int Foobar(func<int> curry) => curry(10);";
        ParseTree? tree = GrammarRules.Parse(code, new Script());
        Assert.IsNotNull(tree);

        PrintFunctionCode(tree);
    }

    [TestMethod]
    public void ValueGetterTest()
    {
        string code = "type Vector { int X, int Y }; int Foobar(Vector vector) => vector->X + vector->Y;";
        ParseTree? tree = GrammarRules.Parse(code, new Script());
        Assert.IsNotNull(tree);

        PrintFunctionCode(tree);
    }

    [TestMethod]
    public void CurryFunctionTest()
    {
        string code = "func<func<int>, int> Foobar(int a) => (int b) => ((int c) => (a + b + c));";
        ParseTree? tree = GrammarRules.Parse(code, new Script());
        Assert.IsNotNull(tree);

        PrintFunctionCode(tree);
    }

    private static void PrintCode(ParseTree tree)
    {
        string headerPath = Path.GetTempFileName();
        string implPath = Path.GetTempFileName();

        {
            using StreamWriter headerFile = new(headerPath);
            using StreamWriter implFile = new(implPath);

            CppCodeGenerator binder = new();
            binder.CompileCpp(tree, headerFile, implFile);
        }

        {
            using StreamReader headerReader = new(headerPath);
            using StreamReader implReader = new(implPath);

            Console.WriteLine("//// header file:");
            Console.WriteLine(headerReader.ReadToEnd());
            Console.WriteLine();
            Console.WriteLine("//// impl file:");
            Console.WriteLine(implReader.ReadToEnd());
        }

        File.Delete(headerPath);
        File.Delete(implPath);
    }

    [TestMethod]
    public void CompilationTest()
    {
        string code = "func<int, int> Foobar(int a) => (int b) =>(a + b);";
        ParseTree? tree = GrammarRules.Parse(code, new Script());
        Assert.IsNotNull(tree);

        PrintCode(tree);
    }

    [TestMethod]
    public void CurryFunctionCompilationTest()
    {
        string code = "func<func<int, int>, int> Foobar(int a) => (int b) => ((int c) => (a + b + c));";
        ParseTree? tree = GrammarRules.Parse(code, new Script());
        Assert.IsNotNull(tree);

        PrintCode(tree);
    }

    [TestMethod]
    public void ValueGetterCompilationTest()
    {
        string code = "type Vector { int X, int Y }; int Foobar(Vector vector) => vector->X + vector->Y;";
        ParseTree? tree = GrammarRules.Parse(code, new Script());
        Assert.IsNotNull(tree);

        PrintCode(tree);
    }

    [TestMethod]
    public void LambdaInvocationTest()
    {
        string code = "int Foobar(func<int, int> lambda, int arg) => lambda(lambda(arg));";
        ParseTree? tree = GrammarRules.Parse(code, new Script());
        Assert.IsNotNull(tree);

        PrintCode(tree);
    }
}
