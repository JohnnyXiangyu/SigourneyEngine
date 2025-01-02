using System.Collections.Immutable;

namespace SemanticMachine.Grammar.Interpretation;

public class TypeMemberGetter : IEvaluatable
{
    public IEvaluatable Subject { get; private set; }
    public string Member { get; private set; }
    public TypeDefinition ReturnType { get; private set; }
    public string Type => ReturnType.Name;

    public TypeMemberGetter(IEvaluatable subject, string member, ImmutableDictionary<string, ISemanticUnit> context)
    {
        TypeDefinition type = TypeDefinition.Resolve(subject.Type, context);
        if (!type.Parameters.TryGetValue(member, out TypeDefinition? paramType))
            throw new Exception($"type {subject.Type} doesn't have mamber {member}");

        Subject = subject;
        ReturnType = paramType;
        Member = member;
    }

    public string PrettyPrint() => $"{Subject.PrettyPrint()}.{Member}";
}
