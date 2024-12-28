#pragma once

#include <functional>
#include <memory>
#include <vector>
#include <string>

namespace OmniParser
{
    class ISymbol
    {
    public:
        virtual bool IsTerminal() = 0;
    };

    class ITerminalSymbol : public ISymbol
    {
    public:
        bool IsTerminal() override { return true; }
        virtual std::shared_ptr<ITerminalSymbol> Accept(std::string token) = 0;
    };

    class INonTerminalSymbol : public ISymbol
    {
    public:
        bool IsTerminal() override { return false; }
    };

    class TerminalSymbol : public ITerminalSymbol
    {
    private:
        std::string m_content;

    public:
        TerminalSymbol(std::string content) : m_content(content) {}

        std::shared_ptr<ITerminalSymbol> Accept(std::string token)
        {
            if (token != m_content)
                return nullptr;

            return std::shared_ptr<ITerminalSymbol>(new TerminalSymbol(m_content));
        }
    };
}
