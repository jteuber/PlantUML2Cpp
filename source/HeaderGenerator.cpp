#include "Class.h"
#include "HeaderGenerator.h"

#include <algorithm>
#include <assert.h>
#include <concepts>
#include <list>
#include <numeric>
#include <ranges>
#include <regex>
#include <set>
#include <utility>

#include <fmt/core.h>

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

    ret += "\n};\n";

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

    return ret + "\n";
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

    return ret + "\n";
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
        rawUsedTypes.insert(m.returnType);
        for (const auto& p : m.parameters) {
            rawUsedTypes.insert(p.type);
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
    const std::regex templateRegex("([a-zA-Z:_]+)<([a-zA-Z:_<>]+)[, ]*([a-zA-Z:_<>]+)?>");
    std::smatch templateMatch;
    for (const auto& type : rawUsedTypes) {
        std::list<std::string> list{type};
        while (!list.empty()) {
            if (std::regex_match(list.front(), templateMatch, templateRegex)) {
                assert(templateMatch.size() >= 3);

                std::ssub_match sub_match  = templateMatch[1];
                std::ssub_match paramMatch = templateMatch[2];
                usedTypes.insert(sub_match.str());
                list.push_back(paramMatch.str());

                if (templateMatch[3].length() > 0) {
                    std::ssub_match paramMatch2 = templateMatch[3];
                    list.push_back(paramMatch2.str());
                }
            } else {
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
            libraryIncludes.insert(type.substr(ns + 1));
        } else {
            localIncludes.insert(type);
        }
    }

    auto libIncludeStrings =
        libraryIncludes | std::views::transform([this](const std::string& inc) { return "#include <" + inc + ">\n"; });
    std::string libIncs = std::accumulate(libIncludeStrings.begin(), libIncludeStrings.end(), std::string());

    auto localIncludeStrings =
        localIncludes | std::views::transform([this](const std::string& inc) { return "#include \"" + inc + "\"\n"; });
    std::string localIncs = std::accumulate(localIncludeStrings.begin(), localIncludeStrings.end(), std::string());

    return libIncs + "\n" + localIncs;
}

std::string HeaderGenerator::methodToString(const Method& m)
{
    std::string ret = m_config->indent + m.returnType + " " + m.name + "(";
    if (!m.parameters.empty()) {
        ret += std::accumulate(
            m.parameters.begin(),
            m.parameters.end(),
            std::string(),
            [](const std::string& acc, const Parameter& param) { return acc + param.type + " " + param.name + ", "; });
        ret.erase(ret.length() - 2);
    }
    return ret + ");\n";
}

std::string HeaderGenerator::variableToString(const Variable& var, Class::Type classType)
{
    std::string varName = var.name;
    bool needsPrefix    = !(classType == Class::Type::Struct && m_config->noMemberPrefixForStructs);
    if (needsPrefix && !varName.starts_with(m_config->memberPrefix)) {
        varName = m_config->memberPrefix + varName;
    }
    return m_config->indent + variableTypeToString(var) + " " + varName + ";\n";
}

std::string HeaderGenerator::variableTypeToString(const Variable& var)
{
    switch (var.source) {
    case Relationship::Aggregation: {
        auto containerIt = m_config->containerByCardinalityAggregation.find(var.cardinality);
        if (containerIt != m_config->containerByCardinalityAggregation.end()) {
            return fmt::format(containerIt->second, var.type);
        }
        return var.type;
    }
    case Relationship::Composition: {
        auto containerIt = m_config->containerByCardinalityComposition.find(var.cardinality);
        if (containerIt != m_config->containerByCardinalityComposition.end()) {
            return fmt::format(containerIt->second, var.type);
        }
        return var.type;
    }
    default:
        return var.type;
    }
}

std::string HeaderGenerator::visibilityToString(Visibility vis)
{
    switch (vis) {
    case Visibility::Protected:
        return "protected:\n";
    case Visibility::Private:
        return "private:\n";
    default:
        return "public:\n";
    }
}