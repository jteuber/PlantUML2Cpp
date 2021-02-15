#pragma once

#include <peg_parser/generator.h>

#include "AbstractVisitor.h"

class Parser
{
public:
    Parser(/* args */);
    ~Parser();

    bool parse(std::string_view input);
    void visitAST(AbstractVisitor &visitor);

private:
    peg_parser::ParserGenerator<void, AbstractVisitor &> g;
    std::shared_ptr<peg_parser::SyntaxTree> m_ast;
};
