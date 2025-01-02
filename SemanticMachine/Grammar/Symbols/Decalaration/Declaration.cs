using ParserFramework;

namespace SemanticMachine.Grammar.Symbols.Decalaration;
public class Declaration : INonTerminal
{
    public static ISymbol[][] Rules => [
        [new FunctionDecalaration()],
        [new TypeDecalaration()]];
}
