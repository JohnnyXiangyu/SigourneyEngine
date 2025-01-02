using ParserFramework;
using System.Collections.Immutable;

namespace SemanticMachine.Grammar.Interpretation;

public interface ISemanticVerifiable
{
    public ISemanticUnit Verify(ParseTree[] children, ImmutableDictionary<string, ISemanticUnit> resolutionContext);
}

public interface ISemanticUnit
{
    public static ISemanticUnit ResolveSymbol(string symbol, ImmutableDictionary<string, ISemanticUnit> context)
    {
        if (!context.TryGetValue(symbol, out ISemanticUnit? resolution))
            throw new Exception($"unresolved symbol: {symbol}");

        return resolution;
    }

    string PrettyPrint();
}
