#pragma once

#include <vector>

#include "File.h"
#include "Generator.h"
#include "HeaderGenerator.h"
#include "PlantUml/SyntaxNode.h"
#include "Translator.h"

namespace Cpp::Enum {
class EnumGenerator : public Generator
{
public:
    std::vector<File> generate(PlantUml::SyntaxNode root) override;

private:
    Translator m_translator;
    HeaderGenerator m_headerGenerator;
};
} // namespace Cpp::Enum
