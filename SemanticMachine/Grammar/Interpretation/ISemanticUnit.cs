using ParserFramework;
using System.Collections.Immutable;

namespace SemanticMachine.Grammar.Interpretation;

public interface ISemanticVerifiable
{
    public ISemanticUnit Verify(ParseTree[] children, ImmutableDictionary<string, ISemanticUnit> resolutionContext);
}

public interface ISemanticUnit
{
    public static ISemanticUnit ResolveSymbol(string symbol, ImmutableDictionary<string, ISemanticUnit> context)
    {
        if (!context.TryGetValue(symbol, out ISemanticUnit? resolution))
            throw new Exception($"unresolved symbol: {symbol}");

        return resolution;
    }
}

// TODO: why do we use string as types? should use numbers 

public record FunctionDefinition(string Name, string ReturnType, string[] ArgTypes);

public record TypeDefinition(string Name, ImmutableDictionary<string, string> Parameters)
{
    public static TypeDefinition Resolve(string name, ImmutableDictionary<string, ISemanticUnit> context)
    {
        ISemanticUnit resolution = ISemanticUnit.ResolveSymbol(name, context);

        if (resolution is not TypeDefinition typeDef)
            throw new Exception($"incorrect symbol reference for {name}, expecting type, got {resolution.GetType().Name}");

        return typeDef;
    }
}

public class TypeMemberGetter : IEvaluatable
{
    public IEvaluatable Subject { get; private set; }
    public string Member { get; private set; }
    public string Type { get; private set; }

    public TypeMemberGetter(IEvaluatable subject, string member, ImmutableDictionary<string, ISemanticUnit> context)
    {
        TypeDefinition type = TypeDefinition.Resolve(subject.Type, context);
        if (!type.Parameters.TryGetValue(member, out string? paramType))
            throw new Exception($"type {subject.Type} doesn't have mamber {member}");

        Subject = subject;
        Type = paramType;
        Member = member;
    }

    public string PrettyPrint() => $"{Subject.PrettyPrint()}->{Member}";
}

public interface IEvaluatable : ISemanticUnit
{
    string Type { get; }
    string PrettyPrint();
}

public class FunctionCall : IEvaluatable
{
    public FunctionDefinition FunctionDefinition { get; private set; }
    public IEvaluatable[] Arguments { get; private set; }

    public string Type => FunctionDefinition.ReturnType;

    public FunctionCall(FunctionDefinition function, IEvaluatable[] arguments)
    {
        // check type
        // TODO: this is a bad implementation; we need a way to coord back to the original file
        if (function.ArgTypes.Length != arguments.Length || function.ArgTypes.Zip(arguments).Any(pair => pair.First != pair.Second.Type))
            throw new Exception($"function parameter doesn't match provided arguments, normalized call site: {(arguments.Length > 0 ? $"{arguments[0]}." : "")}{function.Name}({string.Join(',', arguments.Select(x => x.Type))})");

        FunctionDefinition = function;
        Arguments = arguments;
    }

    public string PrettyPrint() => $"{FunctionDefinition.Name}({string.Join(',', Arguments.Select(arg => arg.PrettyPrint()))})";
}
