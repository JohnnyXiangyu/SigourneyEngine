using System.Collections.Immutable;

namespace SemanticMachine.Grammar.Interpretation;

/// <summary>
/// callable *creation*, not the type used for parameters
/// </summary>
public interface ICallable : IEvaluatable;

public record LambdaWithCaptures(ImmutableList<LazyEvaluatable> Params, IEvaluatable Body) : ICallable
{
    public TypeDefinition Type => new CallableType(Body.Type, Params);

    public IEnumerable<LazyEvaluatable> ArgumentCaptures => Body.ArgumentCaptures.Except(Params);
}

public record FunctionReference(FunctionPrototype Function) : ICallable
{
    public TypeDefinition Type => new CallableType(Function.ReturnType, Function.Params);

    public IEnumerable<LazyEvaluatable> ArgumentCaptures => [];
}
