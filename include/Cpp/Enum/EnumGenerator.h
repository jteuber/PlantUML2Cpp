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
    explicit EnumGenerator(std::shared_ptr<Config> config);
    std::vector<File> generate(PlantUml::SyntaxNode root) override;

private:
    std::shared_ptr<Config> m_config;

    Translator m_translator;
    HeaderGenerator m_headerGenerator;
};
} // namespace Cpp::Enum
