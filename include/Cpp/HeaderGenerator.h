#pragma once

#include <memory>
#include <set>

#include "Config.h"
#include "Cpp/Class.h"

namespace Cpp {

class HeaderGenerator
{
public:
    explicit HeaderGenerator(std::shared_ptr<Config> config);

    std::string generate(const Class& in);

private:
    std::string generateIncludes(const Class& in);

    std::string toString(const std::string& s);
    std::string toString(const Variable& var);
    std::string toString(const Method& m);
    std::string toString(const VisibilityKeyword& s);
    std::string toString(const Separator& s);

    std::string typeToString(const Type& t);

    std::shared_ptr<Config> m_config;
    std::string m_variablePrefix;
};

} // namespace Cpp
