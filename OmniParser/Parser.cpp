#include "Parser.h"
#include "ISymbol.h"
#include "ImmutableVector.h"
#include <functional>

using namespace std;
using namespace OmniParser;

struct AcceptableState
{
public:
    AcceptableState() = default;
    AcceptableState(ImmutableVector<string> inSuffix, ParseTree inProgress, bool inAccepted) : suffix(inSuffix), progress(inProgress), accepted(inAccepted) {}

    ImmutableVector<string> suffix;
    ParseTree progress;
    bool accepted;

    static AcceptableState Reject()
    {
        AcceptableState newState;
        newState.accepted = false;
        return newState;
    }
};

typedef function<AcceptableState(AcceptableState)> Acceptor;

static AcceptableState EmptyAcceptor(const AcceptableState& state)
{
    return state.suffix.Length() == 0 ? state : AcceptableState::Reject();
}

// declarations
static ParseTree ProcessPartialResult(const ParseTree& oldProgress, const AcceptableState& state);
static AcceptableState TestTerminal(shared_ptr<ITerminalSymbol> terminal, ImmutableVector<string> suffix, Acceptor acceptor);
static AcceptableState ProcessNonTerminal(Grammar grammar, shared_ptr<ISymbol> nonTerminal, Acceptor acceptor, AcceptableState state);
static AcceptableState ParseSymbol(Grammar grammar, shared_ptr<OmniParser::ISymbol> symbol, Acceptor acceptor, AcceptableState state);
static AcceptableState ParseAlternative(Grammar grammar, ParseTree oldProgress, Rule remainingRule, Acceptor acceptor, AcceptableState state);
static AcceptableState ParseLevel(Grammar grammar, RuleSet rules, Acceptor acceptor, AcceptableState state);

ParseTree ProcessPartialResult(const ParseTree& oldProgress, const AcceptableState& state)
{
    if (oldProgress.children.Length() == 0)
    {
        return oldProgress;
    }
    else
    {
        ParseTree newProgress;
        newProgress.self = oldProgress.self;
        newProgress.children = oldProgress.children.Add(state.progress);
        return newProgress;
    }
}

AcceptableState TestTerminal(shared_ptr<ITerminalSymbol> terminal, ImmutableVector<string> suffix, Acceptor acceptor)
{
    shared_ptr<ITerminalSymbol> accepted = terminal->Accept(suffix.At(0));

    if (accepted == nullptr)
        return AcceptableState::Reject();

    return acceptor(AcceptableState(suffix.Skip(1), ParseTree(accepted, ImmutableVector<ParseTree>()), true));
}

AcceptableState ProcessNonTerminal(Grammar grammar, shared_ptr<ISymbol> nonTerminal, Acceptor acceptor, AcceptableState state)
{
    return ParseLevel(grammar, grammar(nonTerminal), acceptor, AcceptableState(state.suffix, ParseTree(nonTerminal, ImmutableVector<ParseTree>()), state.accepted));
}

AcceptableState ParseSymbol(Grammar grammar, shared_ptr<OmniParser::ISymbol> symbol, Acceptor acceptor, AcceptableState state)
{
    // TODO: get this done
    return state.suffix.Length() == 0 ?
        AcceptableState::Reject()
            :
            symbol->IsTerminal() ?
                TestTerminal(static_pointer_cast<ITerminalSymbol>(symbol), state.suffix, acceptor)
                :
                ProcessNonTerminal(grammar, symbol, acceptor, state);
}

AcceptableState ParseAlternative(Grammar grammar, ParseTree oldProgress, Rule remainingRule, Acceptor acceptor, AcceptableState state)
{
    ParseTree newProgress = ProcessPartialResult(oldProgress, state);

    return remainingRule.Length() == 0 ?
        acceptor(AcceptableState(state.suffix, newProgress, true))
        :
        ParseSymbol(grammar, remainingRule.At(0),
            [grammar, newProgress, remainingRule, acceptor](AcceptableState inState)
            {
                return ParseAlternative(grammar, newProgress, remainingRule.Skip(1), acceptor, inState);
            }, state);
}

AcceptableState ParseLevel(Grammar grammar, RuleSet rules, Acceptor acceptor, AcceptableState state)
{ 
    if (rules.Length() == 0)
        return AcceptableState::Reject();

    for (int i = 0; i < rules.Length(); i++)
    {
        Rule rule = rules.Take(1).At(0);
        AcceptableState finalState = ParseAlternative(grammar, state.progress, rule, acceptor, state);
        if (finalState.accepted)
            return finalState;
    }

    return AcceptableState::Reject();
}

static ParseResult ResultSelector(AcceptableState state)
{
    return ParseResult(state.progress, state.accepted);
}

ParseResult OmniParser::Parse(Grammar grammar, std::shared_ptr<ISymbol> rootSymbol, ImmutableVector<std::string> tokens)
{
    return ResultSelector(ParseLevel(grammar, grammar(rootSymbol), EmptyAcceptor, AcceptableState(tokens, ParseTree(rootSymbol, ImmutableVector<ParseTree>()), true)));
}
