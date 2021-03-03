#include "HeaderGenerator.h"
#include "PlantUML2Cpp.h"

#include <array>
#include <filesystem>
#include <fstream>
#include <iostream>
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
    std::string input;
    std::ofstream file(filepath, std::ios_base::out | std::ios_base::app);
    if (file.is_open()) {
        file << contents;
        return true;
    } else {
        std::cout << "unable to write to file " << filepath << std::endl;
        return false;
    }
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
            try {
                std::string fileContents = readFullFile(file.path());
                parser.parse(fileContents);

                parser.visitAST(classBuilder);

                for (const auto& c : classBuilder.results()) {
                    std::string header = headerGenerator.generate(c);

                    writeFullFile(path / "include" / (c.name + ".h"), header);
                }
            } catch (peg_parser::InterpreterError& err) {
                std::cout << "unable to parse " << file << std::endl;
            }
        }
    }

    return true;
}
