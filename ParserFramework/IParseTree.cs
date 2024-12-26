using System.Text.Json;
using System.Text.Json.Nodes;

namespace ParserFramework;
public interface IParseTree
{
    public JsonNode? PrettyPrint() => this switch
    {
        Node(ISymbol Parent, IParseTree[] Children) => JsonSerializer.SerializeToNode(new { Parent = Parent.GetType().Name, Children = Children.Select(c => c.PrettyPrint()) }),
        Leaf(ISymbol Self) => JsonSerializer.SerializeToNode(new { Leaf = Self.GetType().Name }),
        _ => throw new Exception("unexpected parse tree node type")
    };
}

public record Node(ISymbol Parent, IParseTree[] Children) : IParseTree;

public record Leaf(ISymbol Self) : IParseTree;
