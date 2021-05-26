#include "PlantUML2Cpp.h"
#include "Cpp/Class/HeaderGenerator.h"
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
                parser.visitAST(Translator);

                auto classes = std::move(Translator).results();
                PostProcessor.process(classes);

                for (const auto& c : classes) {
                    Generator.generate(path, c);
                }
            }
        }
    }

    return true;
}
