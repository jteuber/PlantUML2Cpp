#pragma once

#include <memory>
#include <set>

#include "Config.h"
#include "Cpp/Class/Class.h"

#include "PlantUml/AbstractVisitor.h"
#include "PlantUml/ModelElement.h"

namespace Cpp {
namespace Class {

class Translator : public PlantUml::AbstractVisitor
{
public:
    Translator(std::shared_ptr<Config> config);
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
    Type umlToCppType(PlantUml::Type umlType);
    Type stringToCppType(std::string_view typeString);
    Type decomposeType(std::string_view type);
    std::list<std::string> getEffectiveNamespace(std::list<std::string> umlTypename);
    std::vector<Class>::iterator findClass(std::list<std::string> umlTypename);
    std::string toNamespacedString(std::list<std::string> namespacedType);

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

} // namespace Class
} // namespace Cpp
