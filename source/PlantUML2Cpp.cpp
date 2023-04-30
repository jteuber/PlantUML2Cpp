#include "PlantUML2Cpp.h"
#include "Cpp/Class/ClassGenerator.h"
#include "Cpp/Variant/VariantGenerator.h"
#include "Cpp/Enum/EnumGenerator.h"
#include "peg_parser/interpreter.h"

#include <array>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <numeric>
#include <ranges>

namespace fs = std::filesystem;

std::string readFullFile(const fs::path& filepath)
{
    std::string input;
    std::ifstream file(filepath);
    for (std::array<char, 256> buffer{}; file.getline(&buffer[0], 256);) {
        input.append(buffer.data());
        input.append("\n");
    }

    return input;
}

bool writeFile(const File& file)
{
    if (!file.path.empty() && !fs::exists(file.path)) {
        std::cout << "writing to file " << file.path << std::endl;

        fs::create_directory(file.path.parent_path());

        std::ofstream f(file.path, std::ios_base::out | std::ios_base::app);
        if (f.is_open()) {
            f << file.content;
            return true;
        }

        std::cout << "unable to write to file " << file.path << std::endl;
    }
    return false;
}

PlantUML2Cpp::PlantUML2Cpp()
{
    m_generators.emplace_back(std::make_unique<Cpp::Class::ClassGenerator>(m_config));
    m_generators.emplace_back(std::make_unique<Cpp::Variant::VariantGenerator>(m_config));
    m_generators.emplace_back(std::make_unique<Cpp::Enum::EnumGenerator>(m_config));
}

bool PlantUML2Cpp::run(fs::path path)
{
    auto modelPath = path / "models";

    fs::directory_entry modelsDir(modelPath);
    if (!modelsDir.exists()) {
        std::cout << "No 'models'-directory found in " << path << "! Abort" << std::endl;
        return false;
    }

    auto configFile = modelPath / "config.json";
    if (fs::directory_entry(configFile).exists()) {
        std::cout << "found config file" << std::endl;
    }

    fs::create_directory(path / m_config->includeFolderName);
    fs::create_directory(path / m_config->sourceFolderName);

    for (const auto& file : fs::directory_iterator(modelPath)) {
        if (file.is_regular_file() && file.path().extension() == ".puml") {
            std::cout << "parsing file " << file << std::endl;
            std::string fileContents = readFullFile(file.path());

            PlantUml::Parser parser;
            if (parser.parse(fileContents)) {
                for (const auto& generator : m_generators) {
                    auto files = generator->generate(parser.getAST());

                    for (const auto& f : files) {
                        writeFile(f);
                    }
                }
            }
        }
    }

    return true;
}
