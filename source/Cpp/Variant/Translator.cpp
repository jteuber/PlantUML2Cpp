#include "Cpp/Variant/Translator.h"

namespace Cpp {
namespace Variant {

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
    auto lastEncountered = findClass<Variant>(r.subject, m_results, m_namespaceStack);

    /*if (m_lastEncountered != m_results.end()) {
        switch (r.type) {
        case PlantUml::RelationshipType::Composition: {
            Variable var;
            if (auto containerIt = m_config->containerByCardinalityComposition.find(r.objectCardinality);
                containerIt != m_config->containerByCardinalityComposition.end()) {
                var.type = stringToCppType(fmt::format(containerIt->second, toNamespacedString(r.object)));
            } else {
                var.type = Type{toNamespacedString(r.object)};
            }

            var.name = r.label;
            if (var.name.empty()) {
                var.name    = r.object.back();
                var.name[0] = std::tolower(var.name[0]);
            }
            m_lastEncountered->body.emplace_back(var);
            break;
        }
        default:
            break;
        }
    }*/

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

bool Translator::visit(const PlantUml::Enumerator& k)
{
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

} // namespace Variant
} // namespace Cpp
