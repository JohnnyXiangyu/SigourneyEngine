using ParserFramework;
using SemanticMachine.Grammar.Interpretation;
using SemanticMachine.Grammar.Symbols.Decalaration;
using System.Collections.Immutable;

namespace SemanticMachine.Grammar.Symbols.Expression;

/// <summary>
/// Lambdas are formatted just like functions, with one exception that they are anonymous and carry captures with them.
/// </summary>
public class Lambda : INonTerminal
{
    public static ISymbol[][] Rules => [
        [new TerminalSymbol("("), new ParameterList(), new TerminalSymbol(")"), new TerminalSymbol("=>"), new Expr()]];

    public static LambdaWithCaptures Verify(ParseTree[] children, ImmutableDictionary<string, ISemanticUnit> symbolTable) => children switch
    {
        //[_, ParseTree(ParameterList, ParseTree[] paramsChildren), _, _, ParseTree(Expr, ParseTree[] exprChildren)] => ,    
        _ => throw new Exception("parsing error, Lambda")
    };
}
