namespace SemanticMachine.ParserArchive;
// the lexer produces this
public record Token(string Raw);

public record ArrowSign() : Token("=>");

public record SemiColomn() : Token(";");

public record LeftParenthesis() : Token("(");

public record RightParenthesis() : Token(")");

public record Comma() : Token(",");
