#pragma once

#include <filesystem>
#include <memory>

#include "Cpp/ClassTranslator.h"
#include "Cpp/HeaderGenerator.h"
#include "PlantUml/Parser.h"

class PlantUML2Cpp
{
public:
    bool run(std::filesystem::path path = std::filesystem::current_path());

private:
    std::shared_ptr<Config> config = std::make_shared<Config>();
    PlantUml::Parser parser;
    Cpp::ClassTranslator classBuilder{config};
    Cpp::HeaderGenerator headerGenerator;
};
