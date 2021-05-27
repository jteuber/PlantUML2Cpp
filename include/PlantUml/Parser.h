#pragma once

#include <list>

#include <peg_parser/generator.h>

#include "AbstractVisitor.h"
#include "PlantUml/SyntaxNode.h"

namespace PlantUml {

class AbstractVisitor;
using Expression = peg_parser::Interpreter<SyntaxNode>::Expression;

class Parser
{
public:
    Parser(/* args */);

    bool parse(std::string_view input);
    const SyntaxNode& getAST();

private:
    // helpers
    static std::string toName(Expression e);
    static std::string toName(std::optional<Expression> e);
    static std::string_view removePadding(std::string_view in);

    std::list<std::string> toNamespace(std::string_view sv);
    std::list<std::string> toNamespace(Expression e);
    std::list<std::string> toNamespace(std::optional<Expression> e);

    SyntaxNode evaluateBody(const Expression& e);

    // members
    peg_parser::ParserGenerator<SyntaxNode> g;
    SyntaxNode root;

    std::string namespaceDelimiter = ".";
    std::vector<size_t> newLinePositions; // records the position of the first character of each line
};

} // namespace PlantUml
