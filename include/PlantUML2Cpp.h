#pragma once

#include <filesystem>
#include <memory>

#include "Cpp/ClassBuilder.h"
#include "Cpp/HeaderGenerator.h"
#include "PlantUml/Parser.h"

class PlantUML2Cpp
{
public:
    bool run(std::filesystem::path path = std::filesystem::current_path());

private:
    std::shared_ptr<Cpp::Config> config = std::make_shared<Cpp::Config>();
    PlantUml::Parser parser;
    Cpp::ClassBuilder classBuilder;
    Cpp::HeaderGenerator headerGenerator{config};
};