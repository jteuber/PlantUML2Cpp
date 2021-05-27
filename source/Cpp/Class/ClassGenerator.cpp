#include "Cpp/Class/ClassGenerator.h"

#include <fstream>
#include <iostream>
#include <numeric>

namespace Cpp {
namespace Class {

ClassGenerator::ClassGenerator(std::shared_ptr<Config> config)
    : m_translator(config)
    , m_postProcessor(config)
    , m_headerGenerator(config)
    , m_config(config)
{}

std::vector<File> ClassGenerator::generate(PlantUml::SyntaxNode root)
{
    std::vector<File> files;

    root.visit(m_translator);
    auto classes = std::move(m_translator).results();

    m_postProcessor.process(classes);

    for (auto c : classes) {
        auto nsPath =
            std::accumulate(c.namespaces.begin(), c.namespaces.end(), fs::path(), [](const auto& a, const auto& b) {
                return fs::path(a) / fs::path(b);
            });

        File header;
        header.content = m_headerGenerator.generate(c);
        header.path    = m_config->includeFolderName / nsPath / (c.name + ".h");
        files.emplace_back(std::move(header));

        File source;
        source.content = m_sourceGenerator.generate(c);
        if (!source.content.empty()) {
            source.path = m_config->sourceFolderName / nsPath / (c.name + ".cpp");
        }
    }
}

} // namespace Class
} // namespace Cpp
