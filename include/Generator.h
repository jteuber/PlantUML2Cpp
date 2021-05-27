#pragma once

#include <vector>

#include "File.h"
#include "PlantUml/SyntaxNode.h"

class Generator
{
public:
    virtual std::vector<File> generate(PlantUml::SyntaxNode root) = 0;
};
