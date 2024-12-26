namespace SemanticMachine;

public enum TokenType
{
    Keyword,
    NamedToken,
    Value
}

public record Token(TokenType Type, string Nominal);
