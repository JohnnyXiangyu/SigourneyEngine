namespace SemanticMachine.Grammar.Interpretation;

public record FunctionCall(FunctionPrototype Function, IEvaluatable[] Arguments) : IEvaluatable
{
    public TypeDefinition Type => Function.ReturnType;
}
