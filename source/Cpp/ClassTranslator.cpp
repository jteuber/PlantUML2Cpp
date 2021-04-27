#include "Cpp/ClassTranslator.h"
#include "PlantUml/ModelElement.h"

#include <algorithm>
#include <iostream>
#include <numeric>
#include <ranges>
#include <regex>
#include <string_view>
#include <utility>

#include <fmt/core.h>

namespace Cpp {

const std::vector<Class>& ClassTranslator::results()
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
        var.type     = v.type.back();
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
        method.returnType = m.returnType.empty() ? "void" : m.returnType.back();
        method.isAbstract = m.isAbstract;
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

        case PlantUml::RelationshipType::Composition:
        case PlantUml::RelationshipType::Aggregation: {
            Variable var;
            var.name = r.label;
            var.type = r.object.back();

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
        e.type != PlantUml::ElementType::Interface) {
        return false;
    }

    Class c;
    if (e.type == PlantUml::ElementType::Interface) {
        c.isInterface = true;
    } else if (e.spotLetter == 'S') {
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

bool ClassTranslator::visit(const PlantUml::Parameter& p)
{
    if (m_lastEncounteredClass != m_classes.end()) {
        Parameter param;
        param.name = p.name;
        param.type = p.type.back();
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

std::string ClassTranslator::generateIncludes(const Class& in)
{
    // record all used types
    std::set<std::string> rawUsedTypes;
    for (const auto& p : in.parents) {
        rawUsedTypes.insert(p);
    }
    for (const auto& v : in.variables) {
        rawUsedTypes.insert(variableTypeToString(v));
    }
    for (const auto& m : in.methods) {
        rawUsedTypes.insert(umlToCppType(m.returnType));
        for (const auto& p : m.parameters) {
            rawUsedTypes.insert(umlToCppType(p.type));
        }
    }

    // split all template types
    std::set<std::string> usedTypes;
    for (const auto& type : rawUsedTypes) {
        auto tempSet = decomposeType(type);
        usedTypes.insert(tempSet.begin(), tempSet.end());
    }

    // remove all types that don't need including
    usedTypes.erase("void");
    usedTypes.erase("bool");
    usedTypes.erase("int");
    usedTypes.erase("float");
    usedTypes.erase("double");
    usedTypes.erase("uint");
    usedTypes.erase("unsigned int");

    std::set<std::string> libraryIncludes;
    std::set<std::string> localIncludes;
    for (const auto& type : usedTypes) {
        if (const auto& it = m_config->typeToIncludeMap.find(type); it != m_config->typeToIncludeMap.end()) {
            libraryIncludes.insert(it->second);
        } else if (auto ns = type.find_last_of("::"); ns != std::string::npos) {
            libraryIncludes.insert(type.substr(ns + 1) + ".h");
        } else {
            localIncludes.insert(type + ".h");
        }
    }

    auto libIncludeStrings =
        libraryIncludes | std::views::transform([this](const std::string& inc) { return "#include <" + inc + ">\n"; });
    std::string libIncs = std::accumulate(libIncludeStrings.begin(), libIncludeStrings.end(), std::string());

    auto localIncludeStrings =
        localIncludes | std::views::transform([this](const std::string& inc) { return "#include \"" + inc + "\"\n"; });
    std::string localIncs = std::accumulate(localIncludeStrings.begin(), localIncludeStrings.end(), std::string());

    if (!libIncs.empty() && !localIncs.empty()) {
        libIncs += "\n";
    }

    return libIncs + localIncs;
}

std::string ClassTranslator::methodToString(const Method& m)
{
    std::string ret = m_config->indent + umlToCppType(m.returnType) + " " + m.name + "(";
    if (!m.parameters.empty()) {
        ret += std::accumulate(m.parameters.begin(),
                               m.parameters.end(),
                               std::string(),
                               [this](const std::string& acc, const Parameter& param) {
                                   return acc + umlToCppType(param.type) + " " + param.name + ", ";
                               });
        ret.erase(ret.length() - 2);
    }
    return ret + ");\n";
}

std::string ClassTranslator::variableToString(const Variable& var, Class::Type classType)
{
    std::string varName = var.name;
    if (varName.empty()) {
        varName    = var.type;
        varName[0] = std::tolower(varName[0]);
    }

    bool needsPrefix = !(classType == Class::Type::Struct && m_config->noMemberPrefixForStructs);
    if (needsPrefix && !varName.starts_with(m_config->memberPrefix)) {
        varName = m_config->memberPrefix + varName;
    }
    return m_config->indent + variableTypeToString(var) + " " + varName + ";\n";
}

std::string ClassTranslator::variableTypeToString(const Variable& var)
{
    std::string varType = umlToCppType(var.type);
    switch (var.source) {
    case Relationship::Aggregation: {
        auto containerIt = m_config->containerByCardinalityAggregation.find(var.cardinality);
        if (containerIt != m_config->containerByCardinalityAggregation.end()) {
            return fmt::format(containerIt->second, varType);
        }
        return varType;
    }
    case Relationship::Composition: {
        auto containerIt = m_config->containerByCardinalityComposition.find(var.cardinality);
        if (containerIt != m_config->containerByCardinalityComposition.end()) {
            return fmt::format(containerIt->second, varType);
        }
        return varType;
    }
    default:
        return varType;
    }
}

std::string ClassTranslator::umlToCppType(std::string umlType)
{
    auto tempSet = decomposeType(umlType);
    for (auto type : tempSet) {
        auto it = m_config->umlToCppTypeMap.find(type);
        if (it != m_config->umlToCppTypeMap.end()) {
            umlType = std::regex_replace(umlType, std::regex(type), it->second);
        }
    }

    return umlType;
}

std::set<std::string> ClassTranslator::decomposeType(const std::string& type)
{
    std::set<std::string> usedTypes;
    std::string templateRegexString = "([a-zA-Z:_]+)(?:<([a-zA-Z:_]+)[, ]*([a-zA-Z:_]+)?>)?";
    templateRegexString = "([a-zA-Z:_]+)(?:<" + templateRegexString + "[, ]*(?:" + templateRegexString + ")?>)?";
    const std::regex templateRegex(templateRegexString);
    std::smatch templateMatch;
    std::list<std::string> list{type};
    while (!list.empty()) {
        if (std::regex_match(list.front(), templateMatch, templateRegex)) {
            assert(templateMatch.size() >= 3);

            std::ssub_match sub_match  = templateMatch[1];
            std::ssub_match paramMatch = templateMatch[2];
            if (sub_match.length() > 0)
                usedTypes.insert(sub_match.str());
            list.push_back(paramMatch.str());

            int i = 3;
            while (templateMatch[i].length() > 0) {
                std::ssub_match paramMatch2 = templateMatch[i];
                list.push_back(paramMatch2.str());
                ++i;
            }
        } else if (!list.front().empty()) {
            usedTypes.insert(list.front());
        }
        list.pop_front();
    }

    return usedTypes;
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
