
namespace SemanticMachine.Grammar.Interpretation;

public record FunctionCall(FunctionPrototype Function, IEvaluatable[] Arguments) : IEvaluatable
{
    public TypeDefinition Type => Function.ReturnType;

    public IEnumerable<string> ParamReferences => Arguments.SelectMany(arg => arg.ParamReferences);
}
