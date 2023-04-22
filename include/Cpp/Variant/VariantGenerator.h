#pragma once

#include <vector>

#include "Cpp/Variant/Translator.h"
#include "File.h"
#include "Generator.h"
#include "HeaderGenerator.h"
#include "PlantUml/SyntaxNode.h"

namespace Cpp::Variant {
class VariantGenerator : public Generator
{
public:
    explicit VariantGenerator(std::shared_ptr<Config> config);
    std::vector<File> generate(PlantUml::SyntaxNode root) override;

private:
    std::shared_ptr<Config> m_config;

    HeaderGenerator m_headerGenerator;
};
} // namespace Cpp::Variant
