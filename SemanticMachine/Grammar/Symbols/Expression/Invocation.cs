using ParserFramework;
using SemanticMachine.Grammar.Interpretation;
using System.Collections.Immutable;

namespace SemanticMachine.Grammar.Symbols.Expression;

public record Invocation() : INonTerminal
{
    public static ISymbol[][] Rules => [
        [new Term(), new TerminalSymbol("."), new NamedSymbol(), new TerminalSymbol("("), new ArgumentList(), new TerminalSymbol(")")],
        [new NamedSymbol(), new TerminalSymbol("("), new ArgumentList(), new TerminalSymbol(")")]];

    private static IEvaluatable VerifyCore(ImmutableDictionary<string, ISemanticUnit> context, string functionName, IEvaluatable[] arguments)
    {
        if (!context.TryGetValue(functionName, out ISemanticUnit? foundFunction))
            throw new Exception($"unresolved symbol {functionName}");

        return foundFunction switch
        {
            FunctionPrototype def => def.Call(arguments),
            LazyEvaluatable lazy => lazy.Type switch
            {
                CallableType lambdaParam => lambdaParam.Call(lazy.Name, arguments),
                _ => throw new Exception($"misuse of symbol {functionName}, expecting function, actually is {foundFunction.GetType().Name}")
            },
            TypeDefinition constructor => constructor.Construct(arguments),
            _ => throw new Exception($"misuse of symbol {functionName}, expecting function, actually is {foundFunction.GetType().Name}")
        };
    }

    public static IEvaluatable Verify(ParseTree[] children, ImmutableDictionary<string, ISemanticUnit> context)
    {
        return children switch
        {
        [ParseTree(Term, ParseTree[] subjectChildren), _, ParseTree(NamedSymbol(string function), _), _, ParseTree(ArgumentList, ParseTree[] laterArgsChildren), _] => VerifyCore(context, function, [Term.Verify(subjectChildren, context), .. ArgumentList.Verify(laterArgsChildren, context)]),
        [ParseTree(NamedSymbol(string function), []), _, ParseTree(ArgumentList, ParseTree[] argsChildren), _] => VerifyCore(context, function, ArgumentList.Verify(argsChildren, context)),
            _ => throw new Exception("parsing error, Invocation")
        };
    }
}
