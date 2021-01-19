#pragma once

#include <peg_parser/generator.h>

#include "PlantUML.h"

class Parser
{
public:
    Parser(/* args */);
    ~Parser();

    PlantUML parse(const std::string& input);

private:
    peg_parser::ParserGenerator<void, PlantUML&> g;
};
