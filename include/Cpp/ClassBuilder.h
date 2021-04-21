#pragma once

#include <memory>
#include <set>

#include "Cpp/Class.h"
#include "Cpp/Config.h"

#include "PlantUml/AbstractVisitor.h"
#include "PlantUml/ModelElement.h"

namespace Cpp {

class ClassBuilder : public PlantUml::AbstractVisitor
{
public:
    const std::vector<Class>& results();

    bool visit(const PlantUml::Variable& v) override;
    bool visit(const PlantUml::Method& m) override;
    bool visit(const PlantUml::Relationship& r) override;
    bool visit(const PlantUml::Container& c) override;
    bool visit(const PlantUml::Element& e) override;
    bool visit(const PlantUml::Note& n) override;
    bool visit(const PlantUml::Separator& s) override;
    bool visit(const PlantUml::Enumerator& e) override;
    bool visit(const PlantUml::Parameter& p) override;
    bool visit(const PlantUml::End& e) override;

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
