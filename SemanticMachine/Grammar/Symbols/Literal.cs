using ParserFramework;
using SemanticMachine.Grammar.Interpretation;

namespace SemanticMachine.Grammar;

public record Literal() : INonTerminal
{
    public static ISymbol[][] Rules => [
        [new BooleanLiteral()],
        [new Int32Literal()]];

    public static IEvaluatable Verify(ParseTree[] children) =>
        children switch
        {
            [ParseTree(BooleanLiteral literal, [])] => literal.Verify(),
            [ParseTree(Int32Literal literal, [])] => literal.Verify(),
            _ => throw new Exception("unexpected code path, Literal.Verify")
        };
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

    public IArithmatic Verify() => new LeafArithmatic(new Int32Value(Value));
}

public record BooleanLiteral(bool Value = false) : ITerminal
{
    public ITerminal? Accept(string token) => token.ToLower() switch
    {
        "true" => new BooleanLiteral(true),
        "false" => new BooleanLiteral(false),
        _ => null
    };

    public IArithmatic Verify() => new LeafArithmatic(new BooleanValue(Value));
}
