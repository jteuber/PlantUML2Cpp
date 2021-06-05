#pragma once

#include <Generator.h>
#include <SyntaxNode.h>
#include <vector>

#include "File.h"
#include "HeaderGenerator.h"
#include "Translator.h"

namespace Cpp {
namespace Variant {

class VariantGenerator : public Main::Generator
{
public:
    std::vector<File> generate(PlantUml::SyntaxNode root);
private:
    Translator m_translator;
    HeaderGenerator m_headerGenerator;
};
} // namespace Variant
} // namespace Cpp
