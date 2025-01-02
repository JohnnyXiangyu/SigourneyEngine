namespace SemanticMachine.Grammar.Interpretation;

public class FunctionCall : IEvaluatable
{
    public FunctionDefinition FunctionDefinition { get; private set; }
    public IEvaluatable[] Arguments { get; private set; }

    public string Type => FunctionDefinition.ReturnType.Name;

    public FunctionCall(FunctionDefinition function, IEvaluatable[] arguments)
    {
        // check type
        if (function.Params.Count != arguments.Length || function.Params.Select(p => p.Type).Zip(arguments).Any(pair => pair.First != pair.Second.Type))
            throw new Exception($"function parameter doesn't match provided arguments, normalized call site: {(arguments.Length > 0 ? $"{arguments[0]}." : "")}{function.Name}({string.Join(',', arguments.Select(x => x.Type))})");

        FunctionDefinition = function;
        Arguments = arguments;
    }

    public string PrettyPrint() => $"{FunctionDefinition.Name}({string.Join(',', Arguments.Select(arg => arg.PrettyPrint()))})";
}
