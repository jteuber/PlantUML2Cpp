#pragma once

#include <memory>

#include "Class.h"
#include "Config.h"

class HeaderGenerator
{
public:
    explicit HeaderGenerator(std::shared_ptr<Config> config);

    std::string generate(const Class& in);

private:
    std::string generateMethods(const std::vector<Method>& methods, Visibility vis, std::string prefix);
    std::string generateMembers(const std::vector<Variable>& members, Visibility vis, std::string prefix);
    std::string variableToString(const Variable& var);

private:
    std::shared_ptr<Config> m_config;
};
