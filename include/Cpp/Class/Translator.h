#pragma once

#include <memory>
#include <set>

#include "Config.h"
#include "Cpp/Class/Class.h"
#include "Cpp/Common/TranslatorUtils.h"

#include "PlantUml/AbstractVisitor.h"
#include "PlantUml/ModelElement.h"

namespace Cpp::Class {

class Translator : public PlantUml::AbstractVisitor
{
public:
    explicit Translator(std::shared_ptr<Config> config);
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
    // variables
    PlantUml::Visibility m_lastVisibility = PlantUml::Visibility::Unspecified;
    std::vector<Class> m_classes;
    std::list<std::string> m_namespaceStack;
    std::list<size_t> m_namespaceSizes;
    std::vector<Class>::iterator m_lastEncounteredClass = m_classes.end();
    bool m_lastClassFromExternalDef                     = false;
    std::shared_ptr<Config> m_config;
    Common::TranslatorUtils m_utils;
};

} // namespace Cpp::Class
