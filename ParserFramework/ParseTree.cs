using System.Text.Json;
using System.Text.Json.Nodes;

namespace ParserFramework;

public record ParseTree(ISymbol Symbol, ParseTree[] Children)
{
    public JsonNode? PrettyPrint() => this switch
    {
        ParseTree(ISymbol Parent, ParseTree[] Children) => Children.Length > 0 ? JsonSerializer.SerializeToNode(new { Type = Parent.GetType().Name, Children = Children.Select(c => c.PrettyPrint()) }) : JsonSerializer.SerializeToNode(new { Leaf = Parent.GetType().Name }),
        _ => throw new Exception("unexpected parse tree node type")
    };
}

internal record TreeNode(ISymbol Symbol, ParseTree[] Children) : ParseTree(Symbol, Children);

internal record TreeLeaf(ISymbol Symbol) : ParseTree(Symbol, []);
