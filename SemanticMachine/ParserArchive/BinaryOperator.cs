namespace SemanticMachine.ParserArchive;
public record BinaryOperator(Token Token);

public record Plus() : BinaryOperator(new Token("+"));
