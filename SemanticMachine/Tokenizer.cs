namespace SemanticMachine;
internal class Tokenizer
{
    public static HashSet<string> s_whiteSpaces = [" ", "\n", "\t"];
    public static HashSet<string> s_operators = [",", "(", ")", ".", "=>"];

    /// <summary>
    /// Produces the raw strings for each token
    /// </summary>
    /// <param name="input"></param>
    /// <returns></returns>
    public static IEnumerable<string> ProtoTokenize(string input)
    {
        IEnumerable<string> resultingStrings = [input];
        foreach (string white in s_whiteSpaces)
        {
            resultingStrings = resultingStrings.SelectMany(segment => segment.Split(white).Where(rSegment => rSegment.Length > 0));
        }
        foreach (string operators in s_operators)
        {
            resultingStrings = resultingStrings.SelectMany(segment => segment.Split(operators).SelectMany<string, string>((rSegment, i) => i == 0 ? [rSegment] : [operators, rSegment]).Where(rrSegment => rrSegment.Length > 0));
        }

        return resultingStrings;
    }
}
