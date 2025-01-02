using ParserFramework;

namespace SemanticMachine.Grammar.Symbols.Definition;
public class Definition : INonTerminal
{
    public static ISymbol[][] Rules => [
        [new FunctionDefinition()],
        [new TypeDefiner()]];
}
