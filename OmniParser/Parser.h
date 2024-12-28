#pragma once

#include "ImmutableVector.h"

#include <memory>
#include <vector>
#include <string>
#include <functional>

namespace OmniParser
{
    class ISymbol;

    typedef ImmutableVector<std::shared_ptr<ISymbol>> Rule;
    typedef ImmutableVector<Rule> RuleSet;
    typedef std::function<RuleSet(std::shared_ptr<ISymbol>)> Grammar;
    
    struct ParseTree
    {
    public:
        ParseTree() = default;

        ParseTree(std::shared_ptr<ISymbol> inSelf, ImmutableVector<ParseTree> inChildren) : self(inSelf), children(inChildren) {}

        std::shared_ptr<ISymbol> self = nullptr;
        ImmutableVector<ParseTree> children;
    };

    struct ParseResult
    {
        ParseResult(ParseTree inTree, bool inSuccess) : tree(inTree), success(inSuccess) {}
        ParseTree tree;
        bool success;
    };


    ParseResult Parse(Grammar grammar, std::shared_ptr<ISymbol> rootSymbol, ImmutableVector<std::string> tokens);
}
