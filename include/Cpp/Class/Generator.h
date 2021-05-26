#pragma once

#include <filesystem>
namespace fs = std::filesystem;

#include "Class.h"
#include "HeaderGenerator.h"
#include "SourceGenerator.h"

namespace Cpp {
namespace Class {

class Generator
{
public:
    Generator(std::shared_ptr<Config> config);
    void generate(fs::path basePath, const Class& c);

private:
    bool writeFullFile(const fs::path& filepath, std::string_view contents);

    HeaderGenerator m_headerGenerator;
    SourceGenerator m_sourceGenerator;

    std::shared_ptr<Config> m_config;
};

} // namespace Class
} // namespace Cpp
