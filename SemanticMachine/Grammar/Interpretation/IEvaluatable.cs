namespace SemanticMachine.Grammar.Interpretation;

public interface IEvaluatable : ISemanticUnit
{
    TypeDefinition Type { get; }
}

public record LazyEvaluatable(TypeDefinition ParamType, string Name) : IEvaluatable
{
    public TypeDefinition Type => ParamType;
} 
