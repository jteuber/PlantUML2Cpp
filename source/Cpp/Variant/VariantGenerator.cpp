#include "Cpp/Variant/VariantGenerator.h"

#include <numeric>

#include <filesystem>
namespace fs = std::filesystem;

namespace Cpp::Variant {

VariantGenerator::VariantGenerator(std::shared_ptr<Config> config)
    : m_config(std::move(config))
    , m_headerGenerator(m_config)
{
}

std::vector<File> VariantGenerator::generate(PlantUml::SyntaxNode root)
{
    std::vector<File> files;

    Translator translator(m_config);
    root.visit(translator);
    auto classes = std::move(translator).results();

    for (const auto& c : classes) {
        auto nsPath =
            std::accumulate(c.namespaces.begin(), c.namespaces.end(), fs::path(), [](const auto& a, const auto& b) {
                return fs::path(a) / fs::path(b);
            });

        File header;
        header.content = m_headerGenerator.generate(c);
        header.path    = m_config->headersPath() / nsPath / (c.name + m_config->headerFileExtention());
        files.emplace_back(std::move(header));
    }

    return files;
}

} // namespace Cpp::Variant
