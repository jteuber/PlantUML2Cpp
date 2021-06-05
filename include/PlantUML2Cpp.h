#pragma once

#include <filesystem>
#include <memory>

#include "Config.h"
#include "Generator.h"
#include "PlantUml/Parser.h"

class PlantUML2Cpp
{
public:
    PlantUML2Cpp();
    bool run(std::filesystem::path path = std::filesystem::current_path());

private:
    std::shared_ptr<Config> config = std::make_shared<Config>();
    std::shared_ptr<Generator> generator;
};
