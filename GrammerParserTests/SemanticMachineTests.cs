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

    [TestMethod]
    public void ArrayFunctionTest()
    {
        string code = "int[][] Foobar() => [[1, 2], [3]];";
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

        CppCodeGenerator binder = new();

        foreach (var functionDef in functionDefinitions.Values)
        {
            binder.GenerateFunction(functionDef);
        }

        foreach (string functionPrototype in binder.FunctionPrototypes)
        {
            Console.WriteLine(functionPrototype);
        }

        foreach (string functionHeader in binder.TemplateFunctions)
        {
            Console.WriteLine(functionHeader);
        }

        foreach (string functionDef in binder.FunctionImplementations)
        {
            Console.WriteLine(functionDef);
        }
    }

    [TestMethod]
    public void FoobarTest()
    {
        int[] x = [1, 2, 3];
        int[] y = x.Take(4).ToArray();

        Console.WriteLine(y.Length);
    }
}
