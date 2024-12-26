namespace ParserFramework;

public interface ISymbol { }

public interface ITerminal : ISymbol
{
    bool Accept(string token);
}

public record TerminalSymbol(string Token) : ITerminal
{
    public bool Accept(string token) => token.Equals(Token);
}

public interface INonTerminal : ISymbol;
