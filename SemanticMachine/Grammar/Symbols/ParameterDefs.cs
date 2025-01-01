using ParserFramework;

namespace SemanticMachine.Grammar;

public record ParameterDefs() : INonTerminal
{
    public static ISymbol[][] Rules => [
        [new ParameterDefs(), new TerminalSymbol(","), new NamedSymbol()],
        [new NamedSymbol()]];
}
