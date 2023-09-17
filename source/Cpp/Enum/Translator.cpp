#include "Cpp/Enum/Translator.h"

#include "Common/LogHelpers.h"

namespace Cpp::Enum {
Translator::Translator(std::shared_ptr<Config> config)
    : m_config(std::move(config))
    , m_utils(m_config)

{
}

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
    return false;
}

bool Translator::visit(const PlantUml::Container& c)
{
    auto f__ = FuncTracer();

    if (c.type == PlantUml::ContainerType::Namespace) {
        m_namespaceSizes.push_back(m_namespaceStack.size());
        m_namespaceStack.insert(m_namespaceStack.end(), c.name.begin(), c.name.end());
    }

    return true;
}

bool Translator::visit(const PlantUml::Element& e)
{
    auto f__ = FuncTracer();

    bool process = false;
    if (e.type == PlantUml::ElementType::Enum) {
        Enum en;
        en.name       = e.name.back();
        en.namespaces = m_namespaceStack;
        en.namespaces.insert(en.namespaces.end(), e.name.begin(), e.name.end());
        en.namespaces.pop_back();

        m_results.emplace_back(std::move(en));
        m_lastEncountered = --m_results.end();

        process = true;
    }

    return process;
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
    auto f__ = FuncTracer();

    if (m_lastEncountered != m_results.end()) {
        m_lastEncountered->enumerators.emplace_back(e.name);
    }

    return false;
}

bool Translator::visit(const PlantUml::Parameter& p)
{
    return false;
}

bool Translator::visit(const PlantUml::End& e)
{
    auto f__ = FuncTracer();

    if (e.type == PlantUml::EndType::Namespace) {
        auto size = m_namespaceSizes.back();
        while (m_namespaceStack.size() > size) {
            m_namespaceStack.pop_back();
        }
        m_namespaceSizes.pop_back();
    }

    return true;
}

bool Translator::visit(const PlantUml::Type& t)
{
    return false;
}

std::vector<Enum> Translator::results() &&
{
    return m_results;
}

} // namespace Cpp::Enum
