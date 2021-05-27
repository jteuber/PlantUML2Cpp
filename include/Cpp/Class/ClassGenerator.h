#pragma once

#include <filesystem>
namespace fs = std::filesystem;

#include "Class.h"
#include "File.h"
#include "Generator.h"
#include "HeaderGenerator.h"
#include "PlantUml/SyntaxNode.h"
#include "PostProcessor.h"
#include "SourceGenerator.h"
#include "Translator.h"

namespace Cpp {
namespace Class {

class ClassGenerator : public Generator
{
public:
    ClassGenerator(std::shared_ptr<Config> config);
    std::vector<File> generate(PlantUml::SyntaxNode root) override;

private:
    Translator m_translator;
    PostProcessor m_postProcessor;
    HeaderGenerator m_headerGenerator;
    SourceGenerator m_sourceGenerator;

    std::shared_ptr<Config> m_config;
};

} // namespace Class
} // namespace Cpp
