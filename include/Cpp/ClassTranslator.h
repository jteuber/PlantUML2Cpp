#pragma once

#include <memory>
#include <set>

#include "Config.h"
#include "Cpp/Class.h"

#include "PlantUml/AbstractVisitor.h"
#include "PlantUml/ModelElement.h"

namespace Cpp {

class ClassTranslator : public PlantUml::AbstractVisitor
{
public:
    ClassTranslator(std::shared_ptr<Config> config);
    std::vector<Class> results() &&;

    bool visit(const PlantUml::Variable& v) override;
    bool visit(const PlantUml::Method& m) override;
    bool visit(const PlantUml::Relationship& r) override;
    bool visit(const PlantUml::Container& c) override;
    bool visit(const PlantUml::Element& e) override;
    bool visit(const PlantUml::Note& n) override;
    bool visit(const PlantUml::Separator& s) override;
    bool visit(const PlantUml::Enumerator& e) override;
    bool visit(const PlantUml::Type& t) override;
    bool visit(const PlantUml::Parameter& p) override;
    bool visit(const PlantUml::End& e) override;

private:
    // helper methods
    std::string variableTypeToString(const PlantUml::Variable& var, PlantUml::RelationshipType relationship);
    Type umlToCppType(PlantUml::Type umlType);
    Type decomposeType(std::string_view type);

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
