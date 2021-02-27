#include "Class.h"
#include "HeaderGenerator.h"

#include <algorithm>
#include <concepts>
#include <numeric>
#include <ranges>
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
        if (memberIdentifier == "public_methods") {
            ret += generateMethods(in.methods, Visibility::Public, "public: \n");
        } else if (memberIdentifier == "protected_methods") {
            ret += generateMethods(in.methods, Visibility::Protected, "protected: \n");
        } else if (memberIdentifier == "private_methods") {
            ret += generateMethods(in.methods, Visibility::Private, "private: \n");
        } else if (memberIdentifier == "public_members") {
            ret += generateMembers(in.variables, Visibility::Public, "public: \n");
        } else if (memberIdentifier == "protected_members") {
            ret += generateMembers(in.variables, Visibility::Protected, "protected: \n");
        } else {
            ret += generateMembers(in.variables, Visibility::Private, "private: \n");
        }
    }

    ret += "\n};\n";

    return ret;
}

std::string HeaderGenerator::generateMethods(const std::vector<Method>& methods, Visibility vis, std::string prefix)
{
    std::string ret;
    auto methodView = methods | filterOnVisibility<Method>(vis);
    if (methodView.begin() != methodView.end()) {
        ret = std::move(prefix);
        for (const auto& m : methodView) {
            ret += m_config->indent + m.returnType + " " + m.name + "(";
            if (!m.parameters.empty()) {
                ret += std::accumulate(m.parameters.begin(),
                                       m.parameters.end(),
                                       std::string(),
                                       [](const std::string& acc, const Parameter& param) {
                                           return acc + param.type + " " + param.name + ", ";
                                       });
                ret.erase(ret.length() - 2);
            }
            ret += ");\n";
        }
    }

    return ret;
}

std::string HeaderGenerator::generateMembers(const std::vector<Variable>& members, Visibility vis, std::string prefix)
{
    std::string ret;
    auto memberView = members | filterOnVisibility<Variable>(vis);
    if (memberView.begin() != memberView.end()) {
        ret = std::move(prefix);
        auto memberStrings =
            memberView | std::views::transform([this](const Variable& var) { return variableToString(var); });
        ret += std::reduce(memberStrings.begin(), memberStrings.end());
    }

    return ret;
}

std::string HeaderGenerator::variableToString(const Variable& var)
{
    switch (var.source) {
    case Relationship::Aggregation: {
        auto containerIt = m_config->containerByCardinalityAggregation.find(var.cardinality);
        if (containerIt != m_config->containerByCardinalityAggregation.end()) {
            return fmt::format(containerIt->second, var.type) + " " + var.name + ";\n";
        }
        return m_config->indent + var.type + " " + var.name + ";\n";
    }
    case Relationship::Composition: {
        auto containerIt = m_config->containerByCardinalityComposition.find(var.cardinality);
        if (containerIt != m_config->containerByCardinalityComposition.end()) {
            return fmt::format(containerIt->second, var.type) + " " + var.name + ";\n";
        }
        return m_config->indent + var.type + " " + var.name + ";\n";
    }
    default:
        return m_config->indent + var.type + " " + var.name + ";\n";
    }
}