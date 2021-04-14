#include "Cpp/ClassBuilder.h"
#include "PlantUml/ModelElement.h"

#include <algorithm>
#include <iostream>
#include <numeric>
#include <ranges>

namespace Cpp {

const std::vector<Class>& ClassBuilder::results()
{
    return m_classes;
}

bool ClassBuilder::visit(const PlantUml::Variable& v)
{
    if (!v.element.empty()) {
        m_lastEncounteredClass = std::ranges::find(m_classes, v.element.back(), &Class::name);
    }

    if (m_lastEncounteredClass != m_classes.end()) {
        Variable var;
        auto& c = *m_lastEncounteredClass;

        var.name       = v.name;
        var.type       = v.type.back();
        var.source     = Relationship::Member;
        var.visibility = static_cast<Visibility>(v.visibility);

        if (c.type == Class::Type::Interface) {
            std::cout << "ERROR: Interface with variable encountered" << std::endl;
        }

        c.variables.push_back(var);
    }

    if (!v.element.empty()) {
        m_lastEncounteredClass = m_classes.end();
    }

    return true;
}

bool ClassBuilder::visit(const PlantUml::Method& m)
{
    if (!m.element.empty()) {
        m_lastEncounteredClass     = std::ranges::find(m_classes, m.element.back(), &Class::name);
        m_lastClassFromExternalDef = true;
    }

    if (m_lastEncounteredClass != m_classes.end()) {
        Method method;
        auto& c = *m_lastEncounteredClass;

        method.name       = m.name;
        method.returnType = m.returnType.empty() ? "void" : m.returnType.back();
        method.visibility = static_cast<Visibility>(m.visibility);

        if (c.type == Class::Type::Struct) {
            std::cout << "ERROR: Struct with method encountered" << std::endl;
        }

        c.methods.push_back(method);
    }

    return true;
}

bool ClassBuilder::visit(const PlantUml::Relationship& r)
{
    m_lastEncounteredClass = std::ranges::find(m_classes, r.subject.back(), &Class::name);

    if (m_lastEncounteredClass != m_classes.end()) {
        switch (r.type) {
        case PlantUml::RelationshipType::Extension: {
            if (r.object.front().empty()) {
                m_lastEncounteredClass->parents.push_back(r.object.back());
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

                m_lastEncounteredClass->parents.push_back(dep);
            }
            break;
        }

        case PlantUml::RelationshipType::Composition:
        case PlantUml::RelationshipType::Aggregation: {
            Variable var;
            var.type        = r.object.back();
            var.source      = static_cast<Relationship>(r.type);
            var.cardinality = r.objectCardinality;
            var.name        = r.label;
            m_lastEncounteredClass->variables.push_back(var);
            break;
        }
        default:
            break;
        }
    }

    return true;
}

bool ClassBuilder::visit(const PlantUml::Container& c)
{
    if (c.type == PlantUml::ContainerType::Namespace) {
        m_namespaceSizes.push_back(m_namespaceStack.size());
        m_namespaceStack.insert(m_namespaceStack.end(), c.name.begin(), c.name.end());
    }

    return true;
}

bool ClassBuilder::visit(const PlantUml::Element& e)
{
    Class c;

    if (e.type == PlantUml::ElementType::Abstract) {
        c.type = Class::Type::Abstract;
    } else if (e.type == PlantUml::ElementType::Class) {
        c.type = Class::Type::Class;
    } else if (e.type == PlantUml::ElementType::Entity) {
        c.type = Class::Type::Struct;
    } else if (e.type == PlantUml::ElementType::Interface) {
        c.type = Class::Type::Interface;
    } else {
        return false;
    }

    c.namespaceStack = m_namespaceStack;
    c.name           = e.name.back();
    c.namespaceStack.insert(c.namespaceStack.end(), e.name.begin(), e.name.end());
    c.namespaceStack.pop_back();
    c.stereotype = e.stereotype;

    m_classes.push_back(c);
    m_lastEncounteredClass = --m_classes.end();

    return true;
}

bool ClassBuilder::visit(const PlantUml::Note& /*n*/)
{
    return false;
}

bool ClassBuilder::visit(const PlantUml::Separator& /*s*/)
{
    return false;
}

bool ClassBuilder::visit(const PlantUml::Enumerator& /*e*/)
{
    return false;
}

bool ClassBuilder::visit(const PlantUml::Parameter& p)
{
    if (m_lastEncounteredClass != m_classes.end()) {
        Parameter param;
        param.name = p.name;
        param.type = p.type.back();
        m_lastEncounteredClass->methods.back().parameters.push_back(param);
    }

    return true;
}

bool ClassBuilder::visit(const PlantUml::End& e)
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

} // namespace Cpp
