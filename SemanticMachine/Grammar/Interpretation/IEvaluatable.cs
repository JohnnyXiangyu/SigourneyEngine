namespace SemanticMachine.Grammar.Interpretation;

public interface IEvaluatable : ISemanticUnit
{
    string Type { get; }
    string PrettyPrint();
}

public record LazyEvaluatable(TypeDefinition ParamType, string Name) : IEvaluatable
{
    public string Type => ParamType.Name;

    public string PrettyPrint() => Name;


} 
