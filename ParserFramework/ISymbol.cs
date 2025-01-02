namespace ParserFramework;

public interface ISymbol
{
    public string GrammarPrint() => GetType().Name;
}

public interface ITerminal : ISymbol
{
    ITerminal? Accept(string token);
}

public record TerminalSymbol(string Token) : ITerminal
{
    public ITerminal? Accept(string token) => token.Equals(Token) ? this : null;

    public string GrammarPrint() => $"T\"{Token}\"";
}

public interface INonTerminal : ISymbol;
