using ParserFramework;
using System.Text.RegularExpressions;

namespace SemanticMachine.Grammar;

public static partial class AllSymbols
{
    public record Expr() : INonTerminal;
    public record Invocation() : INonTerminal;
    public record Term() : INonTerminal;

    public record BinOperation() : INonTerminal;
    public record Binop(string Token = "") : ITerminal
    {
        private static readonly HashSet<string> s_BinOps = "+-*/=".Select(c => c.ToString()).ToHashSet();

        public ITerminal? Accept(string token) => s_BinOps.Contains(token) ? this with { Token = token } : null;
    }

    public record Lambda() : INonTerminal;
    public record ParameterDefs() : INonTerminal;
    public record ArgumentList() : INonTerminal;

    public record Literal() : INonTerminal;

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

    public record ValueGetter() : INonTerminal;

    public partial record NamedSymbol(string Name = "") : ITerminal
    {
        [GeneratedRegex(@"[\w]+")]
        private static partial Regex GetPattern();

        public ITerminal? Accept(string token) => GetPattern().IsMatch(token) ? new NamedSymbol(token) : null;
    }
}
