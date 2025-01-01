using ParserFramework;
using SemanticMachine.Grammar.Interpretation;
using System.Collections.Immutable;

namespace SemanticMachine.Grammar;

public record Literal() : INonTerminal
{
    public static ISymbol[][] Rules => [
        [new BooleanLiteral()],
        [new Int32Literal()]];

    public IEvaluatable Verify(ParseTree[] children, ImmutableDictionary<string, ISemanticUnit> resolutionContext)
    {
        throw new NotImplementedException();
    }
}

public record Int32Literal(int Value = 0) : ITerminal
{
    public ITerminal? Accept(string token)
    {
        try
        {
            return new Int32Literal(int.Parse(token));
        }
        catch
        {
            return null;
        }
    }
}

public record BooleanLiteral(bool Value = false) : ITerminal
{
    public ITerminal? Accept(string token) => token.ToLower() switch
    {
        "true" => new BooleanLiteral(true),
        "false" => new BooleanLiteral(false),
        _ => null
    };
}
