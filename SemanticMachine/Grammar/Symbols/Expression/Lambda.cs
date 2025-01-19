using ParserFramework;
using SemanticMachine.Grammar.Interpretation;
using SemanticMachine.Grammar.Symbols.Decalaration;
using System.Collections.Immutable;

namespace SemanticMachine.Grammar.Symbols.Expression;

/// <summary>
/// Lambdas are formatted just like functions, with one exception that they are anonymous and carry captures with them.
/// Note: limitation on lambdas, they need to return a TERM instead of an EXPR, to avoid recursion.
/// </summary>
public class Lambda : INonTerminal
{
    public static ISymbol[][] Rules => [
        [new TerminalSymbol("("), new ParameterList(), new TerminalSymbol(")"), new TerminalSymbol("=>"), new Term()],
        [new NamedSymbol()]];

    private static LambdaWithCaptures VerifyCore(ParseTree[] paramsChildren, ParseTree[] exprChildren, ImmutableDictionary<string, ISemanticUnit> symbolTable)
    {
        ImmutableList<LazyEvaluatable> parameters = ParameterList.Flatten(paramsChildren, symbolTable);
        
        ImmutableDictionary<string, ISemanticUnit> bodySymboltable = symbolTable.AddRange(parameters.Select(param => new KeyValuePair<string, ISemanticUnit>(param.Name, new LazyEvaluatable(param.Type, param.Name))));
        
        IEvaluatable body = Term.Verify(exprChildren, bodySymboltable);

        return new LambdaWithCaptures(parameters, body);
    }

    private static FunctionReference VerifyFunctionReference(string name, ImmutableDictionary<string, ISemanticUnit> symbolTable)
    {
        if (!symbolTable.TryGetValue(name, out var func))
            throw new Exception($"unresolved symbol {name}");

        if (func is not FunctionPrototype prototype)
            throw new Exception($"misuse of symbol {name}, expecting function, actually is {func.GetType().Name}");

        return new FunctionReference(prototype);
    }

    public static ICallable Verify(ParseTree[] children, ImmutableDictionary<string, ISemanticUnit> symbolTable) => children switch
    {
        [_, ParseTree(ParameterList, ParseTree[] paramsChildren), _, _, ParseTree(Term, ParseTree[] exprChildren)] => VerifyCore(paramsChildren, exprChildren, symbolTable),
        [ParseTree(NamedSymbol(string name), [])] => VerifyFunctionReference(name, symbolTable),
        _ => throw new Exception("parsing error, Lambda")
    };
}
