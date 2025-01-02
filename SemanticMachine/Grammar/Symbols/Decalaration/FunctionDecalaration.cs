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

    public static FunctionDefinition Verify(ParseTree[] children, ImmutableDictionary<string, ISemanticUnit> context) => children switch
    {
        [ParseTree(NamedSymbol(string type), []), ParseTree(NamedSymbol(string functionName), []), _, ParseTree(ParameterList, ParseTree[] parametersChildren), _, _, _, ParseTree(Expr, ParseTree[] exprChildren), _] => ParameterList.Flatten(parametersChildren, context) switch
        {
            ImmutableList<LazyEvaluatable> parameters => new Interpretation.FunctionDefinition(
                functionName,
                TypeDefinition.Resolve(type, context), 
                parameters, 
                Expr.Verify(exprChildren, context.AddRange(parameters.Select(param => new KeyValuePair<string, ISemanticUnit>(param.Name, param))))),
            _ => throw new Exception($"parsing error, {nameof(FunctionDecalaration)}")
        },
        _ => throw new Exception($"parsing error, {nameof(FunctionDecalaration)}")
    };
}
