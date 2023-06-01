#include "Cpp/Enum/EnumGenerator.h"

#include <numeric>

#include <filesystem>
namespace fs = std::filesystem;

namespace Cpp::Enum {

EnumGenerator::EnumGenerator(std::shared_ptr<Config> config)
    : m_config(std::move(config))
    , m_translator(m_config)
    , m_headerGenerator(m_config)
{
}

std::vector<File> EnumGenerator::generate(PlantUml::SyntaxNode root)
{
    std::vector<File> files;

    root.visit(m_translator);
    auto classes = std::move(m_translator).results();

    for (const auto& c : classes) {
        auto nsPath =
            std::accumulate(c.namespaces.begin(), c.namespaces.end(), fs::path(), [](const auto& a, const auto& b) {
                return fs::path(a) / fs::path(b);
            });

        File header;
        header.content = m_headerGenerator.generate(c);
        header.path    = m_config->headersPath() / nsPath / (c.name + "." + m_config->headerFileExtention());
        files.emplace_back(std::move(header));
    }

    return files;
}

} // namespace Cpp::Enum
