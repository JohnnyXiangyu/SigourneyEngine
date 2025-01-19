namespace SemanticMachine.Grammar.Interpretation;

public record StaticInvocation(FunctionPrototype Function, IEvaluatable[] Arguments) : IEvaluatable
{
    public TypeDefinition Type => Function.ReturnType;

    public IEnumerable<LazyEvaluatable> ArgumentCaptures => Arguments.SelectMany(arg => arg.ArgumentCaptures);
}
