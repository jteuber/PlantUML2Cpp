#include "Cpp/HeaderGenerator.h"

#include <algorithm>
#include <assert.h>
#include <cctype>
#include <concepts>
#include <list>
#include <numeric>
#include <ranges>
#include <regex>
#include <set>
#include <string_view>
#include <utility>

#include <fmt/core.h>

namespace Cpp {

template <typename T>
concept HasVisibility = requires(T a)
{
    std::same_as<Visibility, decltype(a.visibility)>;
};

HeaderGenerator::HeaderGenerator(std::shared_ptr<Config> config)
    : m_config(std::move(config))
{}

template <HasVisibility T>
auto filterOnVisibility(Visibility vis)
{
    return std::views::filter([vis](const T& m) { return m.visibility == vis; });
}

std::string HeaderGenerator::generate(const Class& in)
{
    std::string ret;

    // pragme once
    ret += "#pragma once\n\n";

    // Includes
    ret += generateIncludes(in);

    ret += "\n";

    // open namespaces
    for (const auto& ns : in.namespaceStack) {
        ret += "namespace " + ns + " {\n";
    }
    ret += "\n";

    // Definintion
    std::string classDef = "class";
    if (in.type == Class::Type::Struct) {
        classDef = "struct";
    }

    classDef += " " + in.name;

    if (!in.parents.empty()) {
        classDef += " : public ";
        classDef = std::accumulate(
            in.parents.begin(), in.parents.end(), classDef, [](const std::string& acc, const std::string& parent) {
                return acc + parent + ", ";
            });
        classDef.erase(classDef.length() - 2);
    }
    ret += classDef + "\n{\n";

    // Body: methods
    for (const auto& memberIdentifier : m_config->memberOrder) {
        if (memberIdentifier.second == "methods") {
            ret += generateMethods(in.methods, memberIdentifier.first);
        } else if (memberIdentifier.second == "variables") {
            ret += generateMembers(in.variables, memberIdentifier.first, in.type);
        }
    }

    ret += "};\n";

    // close namespaces
    for (const auto& ns : in.namespaceStack | std::views::reverse) {
        ret += "} // " + ns + "\n";
    }

    return ret;
}

std::string HeaderGenerator::generateMethods(const std::vector<Method>& methods, Visibility vis)
{
    std::string ret;
    auto methodView = methods | filterOnVisibility<Method>(vis);
    if (methodView.begin() != methodView.end()) {
        ret += visibilityToString(vis);
        auto methodStings = methodView | std::views::transform([this](const Method& m) { return methodToString(m); });
        ret += std::accumulate(methodStings.begin(), methodStings.end(), std::string());
    }

    return ret;
}

std::string
HeaderGenerator::generateMembers(const std::vector<Variable>& members, Visibility vis, Class::Type classType)
{
    std::string ret;
    auto memberView = members | filterOnVisibility<Variable>(vis);
    if (memberView.begin() != memberView.end()) {
        ret += visibilityToString(vis);
        auto memberStrings = memberView | std::views::transform([this, classType](const Variable& var) {
                                 return variableToString(var, classType);
                             });
        ret += std::accumulate(memberStrings.begin(), memberStrings.end(), std::string());
    }

    return ret;
}

std::string HeaderGenerator::generateIncludes(const Class& in)
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

    // remove all types that don't need including
    rawUsedTypes.erase("void");
    rawUsedTypes.erase("bool");
    rawUsedTypes.erase("int");
    rawUsedTypes.erase("float");
    rawUsedTypes.erase("double");
    rawUsedTypes.erase("uint");
    rawUsedTypes.erase("unsigned int");

    // split all template types
    std::set<std::string> usedTypes;
    std::string templateRegexString = "([a-zA-Z:_]+)(?:<([a-zA-Z:_]+)[, ]*([a-zA-Z:_]+)?>)?";
    templateRegexString = "([a-zA-Z:_]+)(?:<" + templateRegexString + "[, ]*(?:" + templateRegexString + ")?>)?";
    const std::regex templateRegex(templateRegexString);
    std::smatch templateMatch;
    for (const auto& type : rawUsedTypes) {
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
    }

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

std::string HeaderGenerator::methodToString(const Method& m)
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

std::string HeaderGenerator::variableToString(const Variable& var, Class::Type classType)
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

std::string HeaderGenerator::variableTypeToString(const Variable& var)
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

std::string HeaderGenerator::umlToCppType(const std::string& umlType)
{
    auto it = m_config->umlToCppTypeMap.find(umlType);
    if (it != m_config->umlToCppTypeMap.end()) {
        return it->second;
    }

    return umlType;
}

std::string HeaderGenerator::visibilityToString(Visibility vis)
{
    switch (vis) {
    case Visibility::Protected:
        return "\nprotected:\n";
    case Visibility::Private:
        return "\nprivate:\n";
    case Visibility::Public:
        return "\npublic:\n";
    default:
        return "";
    }
}

} // namespace Cpp