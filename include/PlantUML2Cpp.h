#pragma once

#include <filesystem>
#include <memory>

#include "Config.h"
#include "Generator.h"
#include "PlantUml/Parser.h"

class PlantUML2Cpp
{
public:
    PlantUML2Cpp(std::shared_ptr<Config> config);
    bool run();

private:
    std::shared_ptr<Config> m_config;
    std::vector<std::unique_ptr<Generator>> m_generators;
};
