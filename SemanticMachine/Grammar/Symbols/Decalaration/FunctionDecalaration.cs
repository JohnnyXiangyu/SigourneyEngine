using ParserFramework;
using SemanticMachine.Grammar.Interpretation;
using SemanticMachine.Grammar.Symbols.Expression;
using System.Collections.Immutable;

namespace SemanticMachine.Grammar.Symbols.Decalaration;

public record ParameterList : INonTerminal
{
    public static ISymbol[][] Rules => [
        [new NamedSymbol(), new NamedSymbol()],
        [new ParameterList(), new TerminalSymbol(","), new NamedSymbol(), new NamedSymbol()]];

    private static ImmutableList<LazyEvaluatable> FlattenCore(ParseTree[] children, ImmutableList<LazyEvaluatable> reversedSuffix, ImmutableDictionary<string, ISemanticUnit> context) => children switch
    {
        [ParseTree(NamedSymbol(string type), []), ParseTree(NamedSymbol(string name), [])] => reversedSuffix.Add(new LazyEvaluatable(TypeDefinition.Resolve(type, context), name)),
        [ParseTree(ParameterList, ParseTree[] listChildren), _, ParseTree(NamedSymbol(string type), []), ParseTree(NamedSymbol(string name), [])] => FlattenCore(listChildren, reversedSuffix.Add(new LazyEvaluatable(TypeDefinition.Resolve(type, context), name)), context),
        _ => throw new Exception("parsing error, ParameterList")
    };

    public static ImmutableList<LazyEvaluatable> Flatten(ParseTree[] children, ImmutableDictionary<string, ISemanticUnit> context) => FlattenCore(children, [], context).Reverse();
}

public record FunctionDecalaration : INonTerminal
{
    public static ISymbol[][] Rules => [
        [new NamedSymbol(), new NamedSymbol(), new TerminalSymbol("("), new ParameterList(), new TerminalSymbol(")"), new TerminalSymbol("=>"), new TerminalSymbol("{"), new Expr(), new TerminalSymbol("}")]];

    public static FunctionPrototype ExtractHeader(ParseTree[] children, ImmutableDictionary<string, ISemanticUnit> context) => children switch
    {
        [ParseTree(NamedSymbol(string type), []), ParseTree(NamedSymbol(string functionName), []), _, ParseTree(ParameterList, ParseTree[] parametersChildren), _, _, _, ParseTree(Expr, ParseTree[] exprChildren), _] => ParameterList.Flatten(parametersChildren, context) switch
        {
            ImmutableList<LazyEvaluatable> parameters => new FunctionPrototype(
                functionName,
                TypeDefinition.Resolve(type, context),
                parameters),
            _ => throw new Exception($"parsing error, {nameof(FunctionDecalaration)}")
        },
        _ => throw new Exception($"parsing error, {nameof(FunctionDecalaration)}")
    };

    public static FunctionDefinition VerifyFullDefinition(ParseTree[] children, ImmutableDictionary<string, ISemanticUnit> context) => children switch
    {
        [_, ParseTree(NamedSymbol(string functionName), []), _, ParseTree(ParameterList, ParseTree[] parametersChildren), _, _, _, ParseTree(Expr, ParseTree[] exprChildren), _] => ISemanticUnit.ResolveSymbol(functionName, context) switch
        {
            FunctionPrototype prototype => new FunctionDefinition(prototype, Expr.Verify(exprChildren, context.AddRange(prototype.Params.Select(param => new KeyValuePair<string, ISemanticUnit>(param.Name, param))))),
            ISemanticUnit unit => throw new Exception($"symbol type mismatch, expecting function prototype, got {unit.GetType().Name}")
        },
        _ => throw new Exception($"parsing error, {nameof(FunctionDecalaration)}")
    };
}
