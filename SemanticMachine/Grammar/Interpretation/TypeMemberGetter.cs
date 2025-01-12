namespace SemanticMachine.Grammar.Interpretation;

public class TypeMemberGetter : IEvaluatable
{
    public IEvaluatable Subject { get; private set; }
    public string Member { get; private set; }
    public TypeDefinition ReturnType { get; private set; }
    public TypeDefinition Type => ReturnType;

    public TypeMemberGetter(IEvaluatable subject, string member)
    {
        TypeDefinition type = subject.Type;
        if (!type.Parameters.TryGetValue(member, out TypeDefinition? paramType))
            throw new Exception($"type {subject.Type} doesn't have mamber {member}");

        Subject = subject;
        ReturnType = paramType;
        Member = member;
    }
}
