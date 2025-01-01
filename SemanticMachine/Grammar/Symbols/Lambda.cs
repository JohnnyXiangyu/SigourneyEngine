using ParserFramework;
using SemanticMachine.Grammar.Symbols;

namespace SemanticMachine.Grammar;

public record Lambda() : INonTerminal
{
    public static ISymbol[][] Rules => [
        [new ParameterDefs(), new TerminalSymbol("=>"), new TerminalSymbol("{"), new Expr(), new TerminalSymbol("}")]];
}
