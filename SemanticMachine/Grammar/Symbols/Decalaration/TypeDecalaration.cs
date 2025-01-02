using ParserFramework;
using SemanticMachine.Grammar.Interpretation;
using System.Collections.Immutable;

namespace SemanticMachine.Grammar.Symbols.Decalaration;

public class SingularMember : INonTerminal
{
    public static ISymbol[][] Rules => [
        [new NamedSymbol(), new NamedSymbol()]];

    public static ImmutableDictionary<string, TypeDefinition> ResolveMember(ParseTree[] children, ImmutableDictionary<string, ISemanticUnit> context) => children switch
    {
        [ParseTree(NamedSymbol(string type), []), ParseTree(NamedSymbol(string name), [])] => ImmutableDictionary<string, TypeDefinition>.Empty.Add(name, TypeDefinition.Resolve(type, context)),
        _ => throw new Exception("parsing error, SingularMember")
    };
}

public class MemberList : INonTerminal
{
    public static ISymbol[][] Rules => [
        [new SingularMember()],
        [new MemberList(), new TerminalSymbol(","), new SingularMember()]];

    public static ImmutableDictionary<string, TypeDefinition> Flatten(ParseTree[] children, ImmutableDictionary<string, ISemanticUnit> context) => children switch
    {
        [ParseTree(SingularMember, ParseTree[] memberChildren)] => SingularMember.ResolveMember(memberChildren, context),
        [ParseTree(MemberList, ParseTree[] memberListChildren), _, ParseTree(SingularMember, ParseTree[] memberChildren)] => Flatten(memberListChildren, context).AddRange(SingularMember.ResolveMember(memberChildren, context)),
        _ => throw new Exception("parsing error, MemberList")
    };
}

public class TypeDecalaration : INonTerminal
{
    public static ISymbol[][] Rules => [
        [new TerminalSymbol("type"), new NamedSymbol(), new TerminalSymbol("{"), new MemberList(), new TerminalSymbol("}")]];

    public static TypeDefinition Verify(ParseTree[] children, ImmutableDictionary<string, ISemanticUnit> context) => children switch
    {
        [_, ParseTree(NamedSymbol(string name), []), _, ParseTree(MemberList, ParseTree[] memberListChildren), _] => new Interpretation.TypeDefinition(name, MemberList.Flatten(memberListChildren, context)),
        _ => throw new Exception("parsing error, TypeDefinition")
    };
}
