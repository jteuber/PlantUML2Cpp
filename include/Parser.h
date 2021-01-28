#pragma once

#include <peg_parser/generator.h>

#include "Visitor.h"

class Parser
{
public:
    Parser(/* args */);
    ~Parser();

    PlantUMLPtr parse(const std::string& input);

private:
    peg_parser::ParserGenerator<void, Visitor&> g;
};
