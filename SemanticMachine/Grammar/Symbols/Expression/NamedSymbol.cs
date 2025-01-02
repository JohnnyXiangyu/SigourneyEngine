using ParserFramework;
using SemanticMachine.Grammar.Interpretation;
using System.Collections.Immutable;
using System.Text.RegularExpressions;

namespace SemanticMachine.Grammar;

public partial record NamedSymbol(string Name = "") : ITerminal
{
    [GeneratedRegex(@"[\w]+")]
    private static partial Regex GetPattern();

    public ITerminal? Accept(string token) => GetPattern().IsMatch(token) ? new NamedSymbol(token) : null;

    public T Verify<T>(ImmutableDictionary<string, ISemanticUnit> context) where T : ISemanticUnit
    {
        if (!context.TryGetValue(Name, out var result))
            throw new Exception($"unresolved symbol: {Name}");

        return result switch
        {
            T correctType => correctType,
            ISemanticUnit incorrectType => throw new Exception($"incorrect symbol type for {Name}, expecting {typeof(T).Name}, got {incorrectType.GetType().Name}")
        };
    }
}
