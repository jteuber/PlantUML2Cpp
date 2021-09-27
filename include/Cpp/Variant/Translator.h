#pragma once

#include <memory>
#include <vector>

#include "PlantUml/AbstractVisitor.h"

#include "Config.h"
#include "Cpp/TranslatorUtils.h"
#include "Variant.h"

namespace Cpp {
namespace Variant {

class Translator : public PlantUml::AbstractVisitor
{
public:
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

    std::vector<Variant> results() &&;

private:
    std::list<std::string> m_namespaceStack;
    std::list<size_t> m_namespaceSizes;

    std::vector<Variant> m_results;
    std::vector<Variant>::iterator m_lastEncountered = m_results.end();

    std::shared_ptr<Config> m_config;
    TranslatorUtils m_utils;
};
} // namespace Variant
} // namespace Cpp
