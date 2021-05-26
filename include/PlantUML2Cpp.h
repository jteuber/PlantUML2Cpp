#pragma once

#include <filesystem>
#include <memory>

#include "Cpp/Class/Generator.h"
#include "Cpp/Class/PostProcessor.h"
#include "Cpp/Class/Translator.h"
#include "PlantUml/Parser.h"

class PlantUML2Cpp
{
public:
    bool run(std::filesystem::path path = std::filesystem::current_path());

private:
    std::shared_ptr<Config> config = std::make_shared<Config>();
    PlantUml::Parser parser;
    Cpp::Class::Translator Translator{config};
    Cpp::Class::PostProcessor PostProcessor{config};
    Cpp::Class::Generator Generator{config};
};
