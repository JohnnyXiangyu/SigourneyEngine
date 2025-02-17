﻿using ParserFramework;
using SemanticMachine.Grammar.Interpretation;
using SemanticMachine.Grammar.Symbols.Decalaration;
using System.Collections.Immutable;

namespace SemanticMachine.Grammar.Symbols.Expression;

public record Script : INonTerminal
{
    public static ISymbol[][] Rules => [
        [new MaybeInlineFunction(), new TerminalSymbol(";")],
        [new TypeDecalaration(), new TerminalSymbol(";")],
        [new Script(), new MaybeInlineFunction(), new TerminalSymbol(";")],
        [new Script(), new TypeDecalaration(), new TerminalSymbol(";")]];

    public static ParseTree[] Flatten(ParseTree[] children) => ((ParseTree[])(children switch
    {
        [ParseTree tail, ParseTree(TerminalSymbol(";"), [])] => [tail],
        [ParseTree(Script, ParseTree[] head), ParseTree tail, ParseTree(TerminalSymbol(";"), [])] => [.. Flatten(head), tail],
        _ => throw new Exception("parsing error, Script")
    })).Where(child => child.Symbol is not TerminalSymbol).ToArray();

    public static ImmutableDictionary<string, ISemanticUnit> LoadTypes(ParseTree[] flattened, ImmutableDictionary<string, ISemanticUnit> context) => flattened.Aggregate(context, (oldContext, child) => child switch
    {
        ParseTree(TypeDecalaration, ParseTree[] decChildren) => TypeDecalaration.Verify(decChildren, oldContext) switch
        {
            TypeDefinition verifiedDef => oldContext.Add(verifiedDef.Name, verifiedDef)
        },
        _ => oldContext
    });

    public static ImmutableDictionary<string, ISemanticUnit> LoadFunctionPrototypes(ParseTree[] flattened, ImmutableDictionary<string, ISemanticUnit> context) => flattened.Aggregate(context, (oldContext, child) => child switch
    {
        ParseTree(MaybeInlineFunction, ParseTree[] decChildren) => MaybeInlineFunction.ExtractHeader(decChildren, oldContext) switch
        {
            FunctionPrototype prototype => oldContext.Add(prototype.Name, prototype)
        },
        _ => oldContext
    });

    public static ImmutableDictionary<string, FunctionDefinition> LoadFunctionDefinitions(ParseTree[] flattened, ImmutableDictionary<string, ISemanticUnit> context) => flattened.Aggregate(ImmutableDictionary<string, FunctionDefinition>.Empty, (oldContext, child) => child switch
    {
        ParseTree(MaybeInlineFunction, ParseTree[] decChildren) => MaybeInlineFunction.VerifyFullDefinition(decChildren, context) switch
        {
            FunctionDefinition prototype => oldContext.Add(prototype.Prototype.Name, prototype)
        },
        _ => oldContext
    });
}
