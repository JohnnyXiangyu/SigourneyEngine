using ParserFramework;
using SemanticMachine.Grammar.Interpretation;
using SemanticMachine.Grammar.Symbols.Expression;
using System.Collections.Immutable;

namespace SemanticMachine.Grammar.Symbols.Decalaration;

public record ParameterList : INonTerminal
{
    public static ISymbol[][] Rules => [
        [new TypeIdentifier(), new NamedSymbol()],
        [new ParameterList(), new TerminalSymbol(","), new TypeIdentifier(), new NamedSymbol()]];

    private static ImmutableList<LazyEvaluatable> FlattenCore(ParseTree[] children, ImmutableList<LazyEvaluatable> reversedSuffix, ImmutableDictionary<string, ISemanticUnit> context) => children switch
    {
        [ParseTree(TypeIdentifier, ParseTree[] typeChildren), ParseTree(NamedSymbol(string name), [])] => reversedSuffix.Add(new LazyEvaluatable(TypeIdentifier.Resolve(typeChildren, context), name)),
        [ParseTree(ParameterList, ParseTree[] listChildren), _, ParseTree(TypeIdentifier, ParseTree[] typeChildren), ParseTree(NamedSymbol(string name), [])] => FlattenCore(listChildren, reversedSuffix.Add(new LazyEvaluatable(TypeIdentifier.Resolve(typeChildren, context), name)), context),
        _ => throw new Exception("parsing error, ParameterList")
    };

    public static ImmutableList<LazyEvaluatable> Flatten(ParseTree[] children, ImmutableDictionary<string, ISemanticUnit> context) => FlattenCore(children, [], context).Reverse();
}

public record MaybeInlineFunction : INonTerminal
{
    public static ISymbol[][] Rules => [
        [new TerminalSymbol("inline"), new FunctionDeclaration()],
        [new FunctionDeclaration()]
        ];

    public static FunctionPrototype ExtractHeader(ParseTree[] children, ImmutableDictionary<string, ISemanticUnit> context) => children switch
    {
        [ParseTree(TerminalSymbol("inline"), []), ParseTree(FunctionDeclaration, ParseTree[] decChildren)] => FunctionDeclaration.ExtractHeader(decChildren, context) with { Inline = true },
        [ParseTree(FunctionDeclaration, ParseTree[] decChildren)] => FunctionDeclaration.ExtractHeader(decChildren, context),
        _ => throw new Exception($"parsing error, {nameof(MaybeInlineFunction)}")
    };

    public static FunctionDefinition VerifyFullDefinition(ParseTree[] children, ImmutableDictionary<string, ISemanticUnit> context) => children.Last() switch
    {
        ParseTree(FunctionDeclaration, ParseTree[] decChildren) => FunctionDeclaration.VerifyFullDefinition(decChildren, context),
        _ => throw new Exception($"parsing error, {nameof(MaybeInlineFunction)}")
    };
}

public record FunctionDeclaration : INonTerminal
{
    public static ISymbol[][] Rules => [
        [new TypeIdentifier(), new NamedSymbol(), new TerminalSymbol("("), new ParameterList(), new TerminalSymbol(")"), new TerminalSymbol("=>"), new Expr()],
        [new TypeIdentifier(), new NamedSymbol(), new TerminalSymbol("("), new TerminalSymbol(")"), new TerminalSymbol("=>"), new Expr()]];

    public static FunctionPrototype ExtractHeader(ParseTree[] children, ImmutableDictionary<string, ISemanticUnit> context) => children switch
    {
        [ParseTree(TypeIdentifier, ParseTree[] typeChildren), ParseTree(NamedSymbol(string functionName), []), _, ParseTree(ParameterList, ParseTree[] parametersChildren), _, _, ParseTree(Expr, ParseTree[] exprChildren)] => ParameterList.Flatten(parametersChildren, context) switch
        {
            ImmutableList<LazyEvaluatable> parameters => new FunctionPrototype(
                functionName,
                false,
                TypeIdentifier.Resolve(typeChildren, context),
                parameters),
            _ => throw new Exception($"parsing error, {nameof(FunctionDeclaration)}")
        },
        [ParseTree(TypeIdentifier, ParseTree[] typeChildren), ParseTree(NamedSymbol(string functionName), []), _, _, _, ParseTree(Expr, ParseTree[] exprChildren)] => new FunctionPrototype(
                functionName,
                false,
                TypeIdentifier.Resolve(typeChildren, context),
                []),
        _ => throw new Exception($"parsing error, {nameof(FunctionDeclaration)}")
    };

    public static FunctionDefinition VerifyFullDefinition(ParseTree[] children, ImmutableDictionary<string, ISemanticUnit> context) => children switch
    {
        [_, ParseTree(NamedSymbol(string functionName), []), _, ParseTree(ParameterList, ParseTree[] parametersChildren), _, _, ParseTree(Expr, ParseTree[] exprChildren)] => ISemanticUnit.ResolveSymbol(functionName, context) switch
        {
            FunctionPrototype prototype => new FunctionDefinition(prototype, Expr.Verify(exprChildren, context.AddRange(prototype.Params.Select(param => new KeyValuePair<string, ISemanticUnit>(param.Name, param))))),
            ISemanticUnit unit => throw new Exception($"symbol type mismatch, expecting function prototype, got {unit.GetType().Name}")
        },
        [_, ParseTree(NamedSymbol(string functionName), []), _, _, _, ParseTree(Expr, ParseTree[] exprChildren)] => ISemanticUnit.ResolveSymbol(functionName, context) switch
        {
            FunctionPrototype prototype => new FunctionDefinition(prototype, Expr.Verify(exprChildren, context)),
            ISemanticUnit unit => throw new Exception($"symbol type mismatch, expecting function prototype, got {unit.GetType().Name}")
        },
        _ => throw new Exception($"parsing error, {nameof(FunctionDeclaration)}")
    };
}
