#include "PlantUML2Cpp.h"
#include "Cpp/HeaderGenerator.h"
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

bool writeFullFile(const fs::path& filepath, std::string_view contents)
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

bool PlantUML2Cpp::run(fs::path path)
{
    auto modelPath = path / "models";

    fs::directory_entry modelsDir(modelPath);
    if (!modelsDir.exists()) {
        std::cout << "No 'models'-directory found in " << modelPath << "! Abort" << std::endl;
        return false;
    }

    auto configFile = modelPath / "config.json";
    if (fs::directory_entry(configFile).exists()) {
        std::cout << "found config file" << std::endl;
    }

    for (const auto& file : fs::directory_iterator(modelPath)) {
        if (file.is_regular_file() && file.path().extension() == ".puml") {
            std::cout << "parsing file " << file << std::endl;
            std::string fileContents = readFullFile(file.path());

            if (parser.parse(fileContents)) {
                parser.visitAST(classBuilder);

                fs::create_directory(path / "include");
                for (const auto& c : classBuilder.results()) {
                    std::string header = headerGenerator.generate(c);

                    auto nsPath = std::accumulate(
                        c.namespaces.begin(), c.namespaces.end(), fs::path(), [](const auto& a, const auto& b) {
                            return fs::path(a) / fs::path(b);
                        });

                    writeFullFile(path / "include" / nsPath / (c.name + ".h"), header);
                }
            }
        }
    }

    return true;
}
