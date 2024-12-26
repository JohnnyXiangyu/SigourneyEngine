using System.Text.RegularExpressions;

namespace SemanticMachine;
public partial record Line(Token Name, Definition Definition)
{
    [GeneratedRegex(@"(?<name>[a-zA-Z_]+)\s*(\|=)\s*(?<definition>[^\s].*)")]
    private static partial Regex GetLineStructurePattern();

    public static Line Parse(string line)
    {
        Match match = GetLineStructurePattern().Match(line);
        if (!match.Success)
            throw new Exception();

        
    }
}
