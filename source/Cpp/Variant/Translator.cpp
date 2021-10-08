#include "Cpp/Variant/Translator.h"

#include <utility>

namespace Cpp::Variant {

Translator::Translator(std::shared_ptr<Config> config)
    : m_config(std::move(config))
    , m_utils(m_config)
{}

bool Translator::visit(const PlantUml::Variable& v)
{
    return false;
}

bool Translator::visit(const PlantUml::Method& m)
{
    return false;
}

bool Translator::visit(const PlantUml::Relationship& r)
{
    auto lastEncountered = Common::findClass<Variant>(r.subject, m_results, m_namespaceStack);

    if (lastEncountered != m_results.end()) {
        lastEncountered->containedTypes.emplace_back(r.object.back());
    }

    return true;
}

bool Translator::visit(const PlantUml::Container& c)
{
    if (c.type == PlantUml::ContainerType::Namespace) {
        m_namespaceSizes.push_back(m_namespaceStack.size());
        m_namespaceStack.insert(m_namespaceStack.end(), c.name.begin(), c.name.end());
    }

    return true;
}

bool Translator::visit(const PlantUml::Element& e)
{
    if (e.spotLetter == 'V' && (e.stereotype == "Variant" || e.stereotype.empty())) {
        Variant v;
        v.name       = e.name.back();
        v.namespaces = m_namespaceStack;
        v.namespaces.insert(v.namespaces.end(), e.name.begin(), e.name.end());
        v.namespaces.pop_back();

        m_results.emplace_back(std::move(v));
        m_lastEncountered = --m_results.end();

        return true;
    }

    return false;
}

bool Translator::visit(const PlantUml::Note& n)
{
    return false;
}

bool Translator::visit(const PlantUml::Separator& s)
{
    return false;
}

bool Translator::visit(const PlantUml::Enumerator& e)
{
    if (m_lastEncountered != m_results.end()) {
        m_lastEncountered->containedTypes.emplace_back(e.name);
    }
    return false;
}

bool Translator::visit(const PlantUml::Parameter& p)
{
    return false;
}

bool Translator::visit(const PlantUml::End& e)
{
    if (e.type == PlantUml::EndType::Namespace) {
        auto size = m_namespaceSizes.back();
        while (m_namespaceStack.size() > size) {
            m_namespaceStack.pop_back();
        }
        m_namespaceSizes.pop_back();
    }

    return true;
}

bool Translator::visit(const PlantUml::Type& /*t*/)
{
    return false;
}

std::vector<Variant> Translator::results() &&
{
    return m_results;
}

} // namespace Cpp::Variant
