namespace ParserFramework;
public interface ITokenizer
{
    string[] Lex(string input);
}

public record WaterfallLexer(string[] Operators, string[] Separators) : ITokenizer
{
    public string[] Lex(string input) =>
        Operators switch
        {
            [string head, .. string[] tail] => input.Split(head).Select(segment => (this with { Operators = tail }).Lex(segment)).SelectMany((ret, i) => i == 0 ? ret : [head, .. ret]).Where(segment => segment.Length > 0).ToArray(),
            [] => Separators switch
            {
                [string head, .. string[] tail] => input.Split(head).Where(segment => segment.Length > 0).SelectMany(segment => (this with { Separators = tail }).Lex(segment)).ToArray(),
                [] => [input]
            }
        };
}
