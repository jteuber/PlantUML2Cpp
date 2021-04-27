#pragma once

#include <memory>
#include <set>

#include "Cpp/Class.h"

namespace Cpp {

class IncludeGatherer
{
public:
    void gather(Class& c);

private:
    // helper methods
    std::string generateIncludes(const Class& in);
    std::string methodToString(const Method& m);
    std::string variableToString(const Variable& var, Class::Type classType);
    std::string variableTypeToString(const Variable& var);
    std::string umlToCppType(std::string umlType);
    std::set<std::string> decomposeType(const std::string& type);

    static std::string visibilityToString(PlantUml::Visibility vis);

    // variables
    PlantUml::Visibility m_lastVisibility = PlantUml::Visibility::Unspecified;
    std::vector<Class> m_classes;
    std::list<std::string> m_namespaceStack;
    std::list<size_t> m_namespaceSizes;
    std::vector<Class>::iterator m_lastEncounteredClass = m_classes.end();
    bool m_lastClassFromExternalDef                     = false;
    std::shared_ptr<Config> m_config;
};

} // namespace Cpp
