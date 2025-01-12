using ParserFramework;
using SemanticMachine.Grammar.Interpretation;
using System.Collections.Immutable;

namespace SemanticMachine.Grammar.Symbols.Expression;

public class ArrayContentList : INonTerminal
{
    public static ISymbol[][] Rules => [
        [new Expr()],
        [new ArrayContentList(), new TerminalSymbol(","), new Expr()]];

    public static IEnumerable<IEvaluatable> Flatten(ParseTree[] children, ImmutableDictionary<string, ISemanticUnit> context) => children switch
    {
        [ParseTree(Expr, ParseTree[] exprChildren)] => [Expr.Verify(exprChildren, context)],
        [ParseTree(ArrayContentList, ParseTree[] listChildren), _, ParseTree(Expr, ParseTree[] exprChildren)] => [.. Flatten(listChildren, context), Expr.Verify(exprChildren, context)],
        _ => throw new Exception($"parsing error, {nameof(ArrayContentList)}")
    };
}

public class ArrayInitializer : INonTerminal
{
    public static ISymbol[][] Rules => [
        [new TerminalSymbol("["), new ArrayContentList(), new TerminalSymbol("]")]];

    public static ArrayLiteral Verify(ParseTree[] children, ImmutableDictionary<string, ISemanticUnit> context) => children switch
    {
        [_, ParseTree(ArrayContentList, ParseTree[] contentChildren), _] => ArrayContentList.Flatten(contentChildren, context).Aggregate(((IEnumerable<IEvaluatable>)[], (TypeDefinition?)null), (seed, newValue) => seed switch
        {
            (IEnumerable<IEvaluatable> old, null) => (old.Append(newValue), newValue.Type),
            (IEnumerable<IEvaluatable> old, TypeDefinition type) => type == newValue.Type ? (old.Append(newValue), type) : throw new Exception("Array element not of the same type, ArrayInitializer.Verify")
        }) switch
            {
                (IEnumerable<IEvaluatable> elements, TypeDefinition type) => new ArrayLiteral(type, elements.ToArray()),
                _ => throw new Exception("compilation error, ArrayInitializer.Verify")
            },
        _ => throw new Exception("parsing error, ArrayInitializer")
    };
}
