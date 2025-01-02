using System.Collections.Immutable;

namespace SemanticMachine.Grammar.Interpretation;

public record FunctionPrototype(string Name, TypeDefinition ReturnType, ImmutableList<LazyEvaluatable> Params) : ISemanticUnit
{
    public FunctionCall Call(IEvaluatable[] arguments)
    {
        // check type
        if (Params.Count != arguments.Length || Params.Select(p => p.Type).Zip(arguments).Any(pair => pair.First != pair.Second.Type))
            throw new Exception($"function parameter doesn't match provided arguments, normalized call site: {(arguments.Length > 0 ? $"{arguments[0]}." : "")}{Name}({string.Join(',', arguments.Select(x => x.Type))})");

        return new FunctionCall(this, arguments);
    }

    public string PrettyPrint() =>
        $"{ReturnType.Name} {Name}({string.Join(", ", Params.Select(p => $"{p.Type} {p.Name}"))});";
}

public record FunctionDefinition(FunctionPrototype Prototype, IEvaluatable Body) : ISemanticUnit
{
    public string PrettyPrint() =>
        $"{Prototype.ReturnType.Name} {Prototype.Name}({string.Join(", ", Prototype.Params.Select(p => $"{p.Type} {p.Name}"))})\n{{\n    return {Body.PrettyPrint()};\n}}\n";
}
