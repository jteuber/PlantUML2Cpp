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
    std::string generateMethods(const std::vector<Method>& methods, Visibility vis);
    std::string generateMembers(const std::vector<Variable>& members, Visibility vis);
    std::string methodToString(const Method& m);
    std::string variableToString(const Variable& var);

    static std::string visibilityToString(Visibility vis);

private:
    std::shared_ptr<Config> m_config;
};
