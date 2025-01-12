namespace SemanticMachine.Grammar.Interpretation;

public record ArrayLiteral(TypeDefinition ElementType, IEvaluatable[] Elements) : IEvaluatable
{
    public TypeDefinition Type => new ArrayType(ElementType);
}
