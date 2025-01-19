namespace SemanticMachine.Grammar.Interpretation;
public record DynamicInvocation(FunctionPrototype Prototype, IEvaluatable[] Arguments) : IEvaluatable
{
    public TypeDefinition Type => Prototype.ReturnType;

    public IEnumerable<LazyEvaluatable> ArgumentCaptures => Arguments.SelectMany(arg => arg.ArgumentCaptures);
}
