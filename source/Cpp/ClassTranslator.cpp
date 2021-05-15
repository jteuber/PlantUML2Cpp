#include "Cpp/ClassTranslator.h"
#include "PlantUml/ModelElement.h"

#include <algorithm>
#include <assert.h>
#include <iostream>
#include <numeric>
#include <ranges>
#include <regex>
#include <string_view>
#include <utility>

#include <fmt/core.h>

namespace Cpp {

ClassTranslator::ClassTranslator(std::shared_ptr<Config> config)
    : m_config(config)
{}

std::vector<Class> ClassTranslator::results() &&
{
    return m_classes;
}

bool ClassTranslator::visit(const PlantUml::Variable& v)
{
    if (!v.element.empty()) {
        m_lastEncounteredClass = std::ranges::find(m_classes, v.element.back(), &Class::name);
    }

    if (m_lastEncounteredClass != m_classes.end()) {
        Variable var;
        auto& c = *m_lastEncounteredClass;

        if (v.visibility != m_lastVisibility) {
            c.body.emplace_back(VisibilityKeyword{visibilityToString(v.visibility)});
            m_lastVisibility = v.visibility;
        }

        var.name     = v.name;
        var.type     = umlToCppType(v.type);
        var.isConst  = v.isConst;
        var.isStatic = v.isStatic;

        c.body.emplace_back(var);
    }

    if (!v.element.empty()) {
        m_lastEncounteredClass = m_classes.end();
    }

    return true;
}

bool ClassTranslator::visit(const PlantUml::Method& m)
{
    if (!m.element.empty()) {
        m_lastEncounteredClass     = std::ranges::find(m_classes, m.element.back(), &Class::name);
        m_lastClassFromExternalDef = true;
    }

    if (m_lastEncounteredClass != m_classes.end()) {
        Method method;
        auto& c = *m_lastEncounteredClass;

        if (m.visibility != m_lastVisibility) {
            c.body.emplace_back(VisibilityKeyword{visibilityToString(m.visibility)});
            m_lastVisibility = m.visibility;
        }

        method.name       = m.name;
        method.returnType = umlToCppType(m.returnType);
        method.isAbstract = m.isAbstract || m_lastEncounteredClass->isInterface;
        method.isConst    = m.isConst;
        method.isStatic   = m.isStatic;

        c.body.emplace_back(method);
    }

    return true;
}

bool ClassTranslator::visit(const PlantUml::Relationship& r)
{
    m_lastEncounteredClass = std::ranges::find(m_classes, r.subject.back(), &Class::name);

    if (m_lastEncounteredClass != m_classes.end()) {
        switch (r.type) {
        case PlantUml::RelationshipType::Extension: {
            if (r.object.front().empty()) {
                m_lastEncounteredClass->inherits.push_back(r.object.back());
            } else {
                std::string parentNS;
                if (!m_namespaceStack.empty()) {
                    parentNS =
                        std::accumulate(++m_namespaceStack.begin(),
                                        m_namespaceStack.end(),
                                        m_namespaceStack.front(),
                                        [](const auto& a, const auto& b) -> std::string { return a + "::" + b; });
                }
                std::string dep = std::accumulate(
                    r.object.begin(), r.object.end(), parentNS, [](const auto& a, const auto& b) -> std::string {
                        return a + (a.empty() ? "" : "::") + b;
                    });

                m_lastEncounteredClass->inherits.push_back(dep);
            }
            break;
        }

        case PlantUml::RelationshipType::Composition: {
            Variable var;
            if (auto containerIt = m_config->containerByCardinalityComposition.find(r.objectCardinality);
                containerIt != m_config->containerByCardinalityComposition.end()) {
                var.type = Type{fmt::format(containerIt->second, r.object.back())};
            } else {
                var.type = Type{r.object.back()};
            }

            var.name = r.label;
            if (var.name.empty()) {
                var.name    = r.object.back();
                var.name[0] = std::tolower(var.name[0]);
            }
            m_lastEncounteredClass->body.emplace_back(var);
            break;
        }
        case PlantUml::RelationshipType::Aggregation: {
            Variable var;
            if (auto containerIt = m_config->containerByCardinalityAggregation.find(r.objectCardinality);
                containerIt != m_config->containerByCardinalityAggregation.end()) {
                var.type = Type{fmt::format(containerIt->second, r.object.back())};
            } else {
                var.type = Type{r.object.back()};
            }

            var.name = r.label;
            if (var.name.empty()) {
                var.name    = r.object.back();
                var.name[0] = std::tolower(var.name[0]);
            }
            m_lastEncounteredClass->body.emplace_back(var);
            break;
        }
        default:
            break;
        }
    }

    return true;
}

bool ClassTranslator::visit(const PlantUml::Container& c)
{
    if (c.type == PlantUml::ContainerType::Namespace) {
        m_namespaceSizes.push_back(m_namespaceStack.size());
        m_namespaceStack.insert(m_namespaceStack.end(), c.name.begin(), c.name.end());
    }

    return true;
}

bool ClassTranslator::visit(const PlantUml::Element& e)
{
    if (e.type != PlantUml::ElementType::Class && e.type != PlantUml::ElementType::Entity &&
        e.type != PlantUml::ElementType::Interface && e.type != PlantUml::ElementType::Abstract) {
        return false;
    }

    Class c;
    if (e.type == PlantUml::ElementType::Interface) {
        c.isInterface = true;
    } else if (e.spotLetter == 'S' || e.type == PlantUml::ElementType::Entity) {
        c.isStruct = true;
    }

    c.name       = e.name.back();
    c.namespaces = m_namespaceStack;
    c.namespaces.insert(c.namespaces.end(), e.name.begin(), e.name.end());
    c.namespaces.pop_back();
    if (!e.implements.empty()) {
        c.inherits.push_back(e.implements.back());
    }
    if (!e.extends.empty()) {
        c.inherits.push_back(e.extends.back());
    }

    m_classes.push_back(c);
    m_lastEncounteredClass = --m_classes.end();

    return true;
}

bool ClassTranslator::visit(const PlantUml::Note& /*n*/)
{
    return false;
}

bool ClassTranslator::visit(const PlantUml::Separator& /*s*/)
{
    return false;
}

bool ClassTranslator::visit(const PlantUml::Enumerator& /*e*/)
{
    return false;
}

bool ClassTranslator::visit(const PlantUml::Type& /*t*/)
{
    return false;
}

bool ClassTranslator::visit(const PlantUml::Parameter& p)
{
    if (m_lastEncounteredClass != m_classes.end()) {
        Parameter param;
        param.name = p.name;
        param.type = umlToCppType(p.type);
        std::get<Method>(m_lastEncounteredClass->body.back()).parameters.push_back(param);
    }

    return true;
}

bool ClassTranslator::visit(const PlantUml::End& e)
{
    if (e.type == PlantUml::EndType::Namespace) {
        auto size = m_namespaceSizes.back();
        while (m_namespaceStack.size() > size) {
            m_namespaceStack.pop_back();
        }
        m_namespaceSizes.pop_back();
    } else if (e.type == PlantUml::EndType::Element) {
        m_lastEncounteredClass = m_classes.end();
    } else if (e.type == PlantUml::EndType::Method) {
        if (m_lastClassFromExternalDef) {
            m_lastEncounteredClass     = m_classes.end();
            m_lastClassFromExternalDef = false;
        }
    }

    return true;
}

Type ClassTranslator::umlToCppType(PlantUml::Type umlType)
{
    Type out;

    out.base = umlType.base.back();
    for (auto ns : umlType.base | std::views::reverse | std::views::drop(1))
        out.base = ns + "::" + out.base;

    auto it = m_config->umlToCppTypeMap.find(out.base);
    if (it != m_config->umlToCppTypeMap.end()) {
        out.base = it->second;
    }

    if (out.base.empty())
        out.base = "void";

    for (auto& param : umlType.templateParams) {
        out.templateParams.push_back(umlToCppType(param));
    }

    return out;
}

std::string ClassTranslator::visibilityToString(PlantUml::Visibility vis)
{
    switch (vis) {
    case PlantUml::Visibility::Protected:
        return "protected:";
    case PlantUml::Visibility::Private:
        return "private:";
    case PlantUml::Visibility::Public:
        return "public:";
    default:
        return "";
    }
}

} // namespace Cpp
