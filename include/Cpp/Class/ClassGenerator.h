#pragma once

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
    explicit ClassGenerator(std::shared_ptr<Config> config);
    std::vector<File> generate(PlantUml::SyntaxNode root) override;

private:
    std::shared_ptr<Config> m_config;

    PostProcessor m_postProcessor;
    HeaderGenerator m_headerGenerator;
    SourceGenerator m_sourceGenerator;
};

} // namespace Class
} // namespace Cpp
