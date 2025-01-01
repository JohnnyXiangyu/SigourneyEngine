using ParserFramework;
using System.Reflection;

namespace SemanticMachine.Grammar;

public class GrammarRules
{
    private static readonly string[] s_Operators = ["=>", "->", "!=", "||", "&&", .. "+-*/=&|().[]{},".Select(c => c.ToString())];
    private static readonly string[] s_Sparators = " \n\t".Select(c => c.ToString()).ToArray();

    public static ITokenizer GetLexer() => new WaterfallLexer(s_Operators, s_Sparators);

    public static readonly IReadOnlyDictionary<Type, ISymbol[][]> s_prebuiltMapping = (Assembly.GetAssembly(typeof(GrammarRules)) ?? throw new Exception("error at assembly discovery")).GetTypes()
        .Where(type => type.GetInterfaces().Contains(typeof(INonTerminal)))
        .Select(type => (type, (ISymbol[][]?) type.GetProperty("Rules", typeof(ISymbol[][]))?.GetValue(null))) // stole this from john lynch
        .Where(pair => pair.Item2 is not null)
        .Select(pair => (pair.type, pair.Item2!))
        .ToDictionary();

    public static IEnumerable<IEnumerable<ISymbol>> GrammarFuncCoreNew(ISymbol target) => s_prebuiltMapping[target.GetType()];

    public static string PrintGrammar() => string.Join('\n', s_prebuiltMapping.Select(pair => $"{pair.Key.Name} |=\n{string.Join('\n', pair.Value.Select(rule => "    " + string.Join(", ", rule.Select(x => x.GrammarPrint()))))}"));

    // there's a problem with C-style coding: the recursive part is on the left side of an invcation (foo.bar).
    // making it almost impossible to parse from left to right without bumping into infinite loops,
    // that's why this function wraps around the real grammar function and reverses each individual rules
    public static ISymbol[][] GrammarFunc(ISymbol target) => GrammarFuncCoreNew(target).Select(rule => rule.Reverse().ToArray()).ToArray();

    public static ParseTree ReverseTreeBack(ParseTree tree) => tree with { Children = tree.Children.Select(ReverseTreeBack).Reverse().ToArray() };
}
