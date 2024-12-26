namespace SemanticMachine.ParserArchive;
public abstract record Expression(IEnumerable<Token> Tokens);

public record Symbol(Token Token) : Expression([Token]);

public record Literal(Token Token) : Expression([Token]);

// a + b
public record BinaryOperation(BinaryOperator Operator, Expression Lhs, Expression Rhs) : Expression([..Lhs.Tokens, Operator.Token, ..Rhs.Tokens]);

public record ParameterDef(Symbol Type, Symbol Name);

// string a => Reverse(a);
public record Lambda(IEnumerable<ParameterDef> Parameters, Expression Body) 
    : Expression([..Parameters.SelectMany<ParameterDef, Token>(x => [x.Type.Token, x.Name.Token]), new ArrowSign(), ..Body.Tokens]);

// Project(source, lambda)
public record Invocation(Symbol Function, IEnumerable<Expression> Parameters) 
    : Expression([Function.Token, new LeftParenthesis(), ..Parameters.SkipLast(1).SelectMany<Expression, Token>(param => [..param.Tokens, new Comma()]), ..Parameters.Last().Tokens, new RightParenthesis()]);

// top level item
public record FunctionDefinition(Symbol FunctionName, Lambda Functionbody);
