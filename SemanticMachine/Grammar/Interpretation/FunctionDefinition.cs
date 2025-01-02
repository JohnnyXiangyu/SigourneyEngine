using System.Collections.Immutable;

namespace SemanticMachine.Grammar.Interpretation;

public record FunctionDefinition(string Name, TypeDefinition ReturnType, ImmutableList<LazyEvaluatable> Params, IEvaluatable Body);
