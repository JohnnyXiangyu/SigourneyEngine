using System.Collections.Immutable;

namespace SemanticMachine.Grammar.Interpretation;

public record FunctionPrototype(string Name, bool Inline, TypeDefinition ReturnType, ImmutableList<LazyEvaluatable> Params) : ISemanticUnit
{
    public FunctionCall Call(IEvaluatable[] arguments)
    {
        // check type
        if (Params.Count != arguments.Length || Params.Select(p => p.Type).Zip(arguments).Any(pair => pair.First != pair.Second.Type))
            throw new Exception($"function parameter doesn't match provided arguments, normalized call site: {(arguments.Length > 0 ? $"{arguments[0]}." : "")}{Name}({string.Join(',', arguments.Select(x => x.Type))})");

        return new FunctionCall(this, arguments);
    }
}

public record FunctionDefinition(FunctionPrototype Prototype, IEvaluatable Body) : ISemanticUnit;
