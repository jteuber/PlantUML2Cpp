#include "Cpp/HeaderGenerator.h"

#include <algorithm>
#include <assert.h>
#include <cctype>
#include <concepts>
#include <list>
#include <numeric>
#include <ranges>
#include <string_view>
#include <utility>

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

} // namespace Cpp