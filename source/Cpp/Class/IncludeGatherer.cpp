#include "Cpp/Class/IncludeGatherer.h"

#include <algorithm>
#include <numeric>
#include <variant>

namespace Cpp {
namespace Class {

IncludeGatherer::IncludeGatherer(std::shared_ptr<Config> config)
    : m_config(config)
{}

void IncludeGatherer::gather(Class& c)
{
    // record all used types
    std::set<std::string> usedTypes;
    for (const auto& p : c.inherits) {
        usedTypes.insert(p);
    }
    for (const auto& v : c.body) {
        if (std::holds_alternative<Variable>(v)) {
            auto var    = std::get<Variable>(v);
            auto retSet = decomposeType(var.type);
            usedTypes.insert(retSet.begin(), retSet.end());
        } else if (std::holds_alternative<Method>(v)) {
            auto m      = std::get<Method>(v);
            auto retSet = decomposeType(m.returnType);
            usedTypes.insert(retSet.begin(), retSet.end());
            for (const auto& p : m.parameters) {
                auto retSet = decomposeType(p.type);
                usedTypes.insert(retSet.begin(), retSet.end());
            }
        }
    }

    // remove all types that don't need including
    usedTypes.erase("void");
    usedTypes.erase("bool");
    usedTypes.erase("int");
    usedTypes.erase("float");
    usedTypes.erase("double");
    usedTypes.erase("uint");
    usedTypes.erase("unsigned int");

    for (const auto& type : usedTypes) {
        if (const auto& it = m_config->typeToIncludeMap.find(type); it != m_config->typeToIncludeMap.end()) {
            c.externalIncludes.insert(it->second);
        } else if (auto ns = type.find_last_of("::"); ns != std::string::npos) {
            c.externalIncludes.insert(type.substr(ns + 1) + ".h");
        } else {
            c.localIncludes.insert(type + ".h");
        }
    }
}

std::set<std::string> IncludeGatherer::decomposeType(const Type& type)
{
    std::set<std::string> out{type.base};
    for (auto& param : type.templateParams) {
        auto paramSet = decomposeType(param);
        out.insert(paramSet.begin(), paramSet.end());
    }

    return out;
}

} // namespace Class
} // namespace Cpp
