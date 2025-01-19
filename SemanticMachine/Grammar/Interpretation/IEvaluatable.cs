namespace SemanticMachine.Grammar.Interpretation;

public interface IEvaluatable : ISemanticUnit
{
    TypeDefinition Type { get; }

    public IEnumerable<LazyEvaluatable> ArgumentCaptures { get; }
}

public record LazyEvaluatable(TypeDefinition ParamType, string Name) : IEvaluatable
{
    public TypeDefinition Type => ParamType;

    public IEnumerable<LazyEvaluatable> ArgumentCaptures => [this];
} 
