using ParserFramework;

using static SemanticMachine.Grammar.AllSymbols;

namespace SemanticMachine.Grammar;

public class GrammarRules
{
    private static readonly string[] s_Operators = ["=>", "->", .. "+-*/=!().[]{},".Select(c => c.ToString())];
    private static readonly string[] s_Sparators = " \n\t".Select(c => c.ToString()).ToArray();

    public static ITokenizer GetLexer() => new WaterfallLexer(s_Operators, s_Sparators);

    // there's a problem with C-style coding: the recursive part is on the left side of an invcation (foo.bar).
    // making it almost impossible to parse from left to right without bumping into infinite loops,
    // that's why this function wraps around the real grammar function and reverses each individual rules
    public static ISymbol[][] GrammarFunc(ISymbol target) => GrammarFuncCore(target).Select(rule => rule.Reverse().ToArray()).ToArray();

    public static IEnumerable<IEnumerable<ISymbol>> GrammarFuncCore(ISymbol target) => target switch
    {
        Expr => [
            [new BinOperation()],
            [new Term()],
            [new Lambda()],
            [new ValueGetter()]],
        Invocation => [
            [new Term(), new TerminalSymbol("."), new NamedSymbol(), new TerminalSymbol("("), new ArgumentList(), new TerminalSymbol(")")],
            [new NamedSymbol(), new TerminalSymbol("("), new ArgumentList(), new TerminalSymbol(")")]],
        BinOperation => [
            [new Expr(), new Binop(), new Term()]],
        ArgumentList => [
            [new ArgumentList(), new TerminalSymbol(","), new Expr()],
            [new Expr()],
            [new TerminalSymbol("_")]],
        Literal => [
            [new BooleanLiteral()],
            [new Int32Literal()]],
        Term => [
            [new Literal()],
            [new NamedSymbol()],
            [new TerminalSymbol("("), new Expr(), new TerminalSymbol(")")],
            [new Invocation()],
            [new ValueGetter()]],
        ValueGetter => [
            [new Term(), new TerminalSymbol("->"), new NamedSymbol()]],
        Lambda => [
            [new ParameterDefs(), new TerminalSymbol("=>"), new TerminalSymbol("{"), new Expr(), new TerminalSymbol("}")]],
        ParameterDefs => [
            [new ParameterDefs(), new TerminalSymbol(","), new NamedSymbol()],
            [new NamedSymbol()]],
        _ => throw new Exception("unexpected target symbol type for this language")
    };
}
