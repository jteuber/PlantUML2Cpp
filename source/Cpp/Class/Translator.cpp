#include "Cpp/Class/Translator.h"
#include "Cpp/Common/TranslatorUtils.h"
#include "PlantUml/ModelElement.h"

#include <algorithm>
#include <iostream>
#include <numeric>
#include <ranges>
#include <regex>
#include <string_view>
#include <utility>

#include <fmt/core.h>

#include "Common/LogHelpers.h"

namespace Cpp::Class {

Translator::Translator(std::shared_ptr<Config> config)
    : m_config(std::move(config))
    , m_utils(m_config)
{
}

std::vector<Class> Translator::results() &&
{
    return m_classes;
}

bool Translator::visit(const PlantUml::Variable& v)
{
    FuncTracer f_;

    if (!v.element.empty()) {
        m_lastEncounteredClass = std::ranges::find(m_classes, v.element.back(), &Class::name);
    }

    if (m_lastEncounteredClass != m_classes.end()) {
        Variable var;
        auto& c = *m_lastEncounteredClass;

        if (v.visibility != m_lastVisibility) {
            c.body.emplace_back(VisibilityKeyword{Common::TranslatorUtils::visibilityToString(v.visibility)});
            m_lastVisibility = v.visibility;
        }

        var.name     = v.name;
        var.type     = m_utils.umlToCppType(v.type);
        var.isConst  = v.isConst;
        var.isStatic = v.isStatic;

        c.body.emplace_back(var);
    }

    if (!v.element.empty()) {
        m_lastEncounteredClass = m_classes.end();
    }

    return true;
}

bool Translator::visit(const PlantUml::Method& m)
{
    FuncTracer f_;

    if (!m.element.empty()) {
        m_lastEncounteredClass     = std::ranges::find(m_classes, m.element.back(), &Class::name);
        m_lastClassFromExternalDef = true;
    }

    if (m_lastEncounteredClass != m_classes.end()) {
        if (m.visibility != m_lastVisibility) {
            m_lastEncounteredClass->body.emplace_back(
                VisibilityKeyword{Common::TranslatorUtils::visibilityToString(m.visibility)});
            m_lastVisibility = m.visibility;
        }

        Method method;
        method.name       = m.name;
        method.returnType = m_utils.umlToCppType(m.returnType);
        method.isAbstract = m.isAbstract || m_lastEncounteredClass->isInterface;
        method.isConst    = m.isConst;
        method.isStatic   = m.isStatic;

        m_lastEncounteredClass->body.emplace_back(method);
    }

    return true;
}

bool Translator::visit(const PlantUml::Relationship& r)
{
    FuncTracer f_;

    m_lastEncounteredClass = Common::findClass<Class>(r.subject, m_classes, m_namespaceStack);

    if (m_lastEncounteredClass != m_classes.end()) {
        switch (r.type) {
        case PlantUml::RelationshipType::Extension: {
            m_lastEncounteredClass->inherits.push_back(Common::toNamespacedString(r.object));
            break;
        }

        case PlantUml::RelationshipType::Composition: {
            Variable var;
            if (auto containerIt = m_config->containerByCardinalityComposition().find(r.objectCardinality);
                containerIt != m_config->containerByCardinalityComposition().end()) {
                var.type = m_utils.stringToCppType(
                    fmt::format(fmt::runtime(containerIt->second), Common::toNamespacedString(r.object)));
            } else {
                var.type = Common::Type{Common::toNamespacedString(r.object)};
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
            if (auto containerIt = m_config->containerByCardinalityAggregation().find(r.objectCardinality);
                containerIt != m_config->containerByCardinalityAggregation().end()) {
                var.type = m_utils.stringToCppType(
                    fmt::format(fmt::runtime(containerIt->second), Common::toNamespacedString(r.object)));
            } else {
                var.type = Common::Type{Common::toNamespacedString(r.object)};
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

bool Translator::visit(const PlantUml::Container& c)
{
    FuncTracer f_;

    if (c.type == PlantUml::ContainerType::Namespace) {
        m_namespaceSizes.push_back(m_namespaceStack.size());
        m_namespaceStack.insert(m_namespaceStack.end(), c.name.begin(), c.name.end());
    }

    return true;
}

bool Translator::visit(const PlantUml::Element& e)
{
    FuncTracer f_;

    bool process = true;
    if (e.type != PlantUml::ElementType::Class && e.type != PlantUml::ElementType::Entity &&
        e.type != PlantUml::ElementType::Interface && e.type != PlantUml::ElementType::Abstract) {
        process = false;
    }

    // don't process variants
    if (e.spotLetter == 'V' && (e.stereotype == "Variant" || e.stereotype.empty())) {
        process = false;
    }

    if (process) {
        Class c;
        if (e.type == PlantUml::ElementType::Interface) {
            c.isInterface = true;
        } else if ((e.spotLetter == 'S' && (e.stereotype == "Struct" || e.stereotype.empty())) ||
                   e.type == PlantUml::ElementType::Entity) {
            c.isStruct = true;
        }

        c.name       = e.name.back();
        c.namespaces = m_namespaceStack;
        c.namespaces.insert(c.namespaces.end(), e.name.begin(), e.name.end());
        c.namespaces.pop_back();
        if (!e.implements.empty()) {
            c.inherits.push_back(Common::toNamespacedString(e.implements));
        }
        if (!e.extends.empty()) {
            c.inherits.push_back(Common::toNamespacedString(e.extends));
        }

        m_classes.push_back(c);
        m_lastEncounteredClass = --m_classes.end();
    }

    return process;
}

bool Translator::visit(const PlantUml::Note& /*n*/)
{
    return false;
}

bool Translator::visit(const PlantUml::Separator& /*s*/)
{
    return false;
}

bool Translator::visit(const PlantUml::Enumerator& /*e*/)
{
    return false;
}

bool Translator::visit(const PlantUml::Type& /*t*/)
{
    return false;
}

bool Translator::visit(const PlantUml::Parameter& p)
{
    FuncTracer f_;

    if (m_lastEncounteredClass != m_classes.end()) {
        Parameter param;
        param.name = p.name;
        param.type = m_utils.umlToCppType(p.type);
        std::get<Method>(m_lastEncounteredClass->body.back()).parameters.push_back(param);
    }

    return true;
}

bool Translator::visit(const PlantUml::End& e)
{
    FuncTracer f_;

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

} // namespace Cpp::Class
