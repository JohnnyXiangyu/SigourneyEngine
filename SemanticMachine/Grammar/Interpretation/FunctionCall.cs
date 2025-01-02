namespace SemanticMachine.Grammar.Interpretation;

public class FunctionCall(FunctionPrototype Function, IEvaluatable[] Arguments) : IEvaluatable
{
    public string Type => Function.ReturnType.Name;

    public string PrettyPrint() => $"{Function.Name}({string.Join(',', Arguments.Select(arg => arg.PrettyPrint()))})";
}
