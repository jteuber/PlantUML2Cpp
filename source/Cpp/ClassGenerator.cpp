#include "Cpp/ClassGenerator.h"

#include <fstream>
#include <iostream>
#include <numeric>

namespace Cpp {

ClassGenerator::ClassGenerator(std::shared_ptr<Config> config)
    : m_headerGenerator(config)
    , m_config(config)
{}

void ClassGenerator::generate(fs::path basePath, const Class& c)
{
    auto nsPath =
        std::accumulate(c.namespaces.begin(), c.namespaces.end(), fs::path(), [](const auto& a, const auto& b) {
            return fs::path(a) / fs::path(b);
        });

    fs::create_directory(basePath / m_config->includeFolderName);
    fs::create_directory(basePath / m_config->sourceFolderName);

    std::string header = m_headerGenerator.generate(c);
    writeFullFile(basePath / m_config->includeFolderName / nsPath / (c.name + ".h"), header);

    std::string source = m_sourceGenerator.generate(c);
    if (!source.empty()) {
        writeFullFile(basePath / m_config->sourceFolderName / nsPath / (c.name + ".cpp"), source);
    }
}

bool ClassGenerator::writeFullFile(const fs::path& filepath, std::string_view contents)
{
    if (!fs::exists(filepath)) {
        std::cout << "writing to file " << filepath << std::endl;

        fs::create_directory(filepath.parent_path());

        std::ofstream file(filepath, std::ios_base::out | std::ios_base::app);
        if (file.is_open()) {
            file << contents;
            return true;
        }

        std::cout << "unable to write to file " << filepath << std::endl;
    }
    return false;
}

} // namespace Cpp
