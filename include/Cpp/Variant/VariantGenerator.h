#pragma once

#include <vector>

#include "Cpp/Variant/Translator.h"
#include "File.h"
#include "Generator.h"
#include "HeaderGenerator.h"
#include "PlantUml/SyntaxNode.h"

namespace Cpp {
namespace Variant {

class VariantGenerator : public Generator
{
public:
    std::vector<File> generate(PlantUml::SyntaxNode root);

private:
    Translator m_translator;
    HeaderGenerator m_headerGenerator;
};
} // namespace Variant
} // namespace Cpp
