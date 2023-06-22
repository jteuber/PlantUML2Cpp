#include "Cpp/Class/HeaderGenerator.h"

#include <algorithm>
#include <cctype>
#include <concepts>
#include <list>
#include <numeric>
#include <ranges>
#include <string_view>
#include <utility>

namespace Cpp::Class {

// template <typename T>
// concept HasVisibility = requires(T a)
//{
//    std::same_as<Visibility, decltype(a.visibility)>;
//};

// template <HasVisibility T>
// auto filterOnVisibility(Visibility vis)
//{
//    return std::views::filter([vis](const T& m) { return m.visibility == vis; });
//}

HeaderGenerator::HeaderGenerator(std::shared_ptr<Config> config)
    : m_config(std::move(config))
    , m_genUtils(m_config)
{
}

std::string HeaderGenerator::generate(const Class& in)
{
    // setup
    m_variablePrefix = in.isStruct && m_config->noMemberPrefixForStructs() ? "" : m_config->memberPrefix();

    std::string ret;

    // pragme once
    ret += "#pragma once\n\n";

    // Includes
    ret += generateIncludes(in);

    ret += "\n";

    // open namespaces
    ret += m_genUtils.openNamespaces(in.namespaces);

    // Definintion
    std::string classDef = "class";
    if (in.isStruct) {
        classDef = "struct";
    }

    classDef += " " + in.name;

    if (!in.inherits.empty()) {
        classDef += " : public ";
        classDef = std::accumulate(
            in.inherits.begin(), in.inherits.end(), classDef, [](const std::string& acc, const std::string& parent) {
                return acc + parent + ", ";
            });
        classDef.erase(classDef.length() - 2);
    }
    ret += classDef + "\n{\n";

    // Body
    for (const auto& elem : in.body) {
        ret += std::visit([this](auto&& arg) -> std::string { return toString(arg); }, elem);
        ret += "\n";
    }

    ret += "};\n";

    // close namespaces
    ret += m_genUtils.closeNamespaces(in.namespaces);

    return ret;
}

std::string HeaderGenerator::generateIncludes(const Class& in)
{
    auto libIncludeStrings =
        in.externalIncludes | std::views::transform([](const std::string& inc) { return "#include <" + inc + ">\n"; });
    std::string libIncs = std::accumulate(libIncludeStrings.begin(), libIncludeStrings.end(), std::string());

    auto localIncludeStrings =
        in.localIncludes | std::views::transform([](const std::string& inc) { return "#include \"" + inc + "\"\n"; });
    std::string localIncs = std::accumulate(localIncludeStrings.begin(), localIncludeStrings.end(), std::string());

    if (!libIncs.empty() && !localIncs.empty()) {
        libIncs += "\n";
    }

    return libIncs + localIncs;
}

std::string HeaderGenerator::toString(const std::string& s)
{
    return s;
}

std::string HeaderGenerator::toString(const Variable& var)
{
    std::string varName = m_variablePrefix + var.name;
    return m_config->indent() + typeToString(var.type) + " " + varName + ";";
}

std::string HeaderGenerator::toString(const Method& m)
{
    std::string ret = m_config->indent();
    if (m.isAbstract) {
        ret += "virtual ";
    }
    ret += typeToString(m.returnType) + " " + m.name + "(";
    if (!m.parameters.empty()) {
        ret += std::accumulate(m.parameters.begin(),
                               m.parameters.end(),
                               std::string(),
                               [this](const std::string& acc, const Parameter& param) {
                                   return acc + typeToString(param.type) + " " + param.name + ", ";
                               });
        ret.erase(ret.length() - 2);
    }
    if (m.isAbstract) {
        return ret + ") = 0;";
    }
    return ret + ");";
}

std::string HeaderGenerator::toString(const VisibilityKeyword& s)
{
    return s.name;
}

std::string HeaderGenerator::toString(const Separator& s)
{
    return "// " + s.text;
}

std::string HeaderGenerator::typeToString(const Common::Type& t)
{
    std::string templ;
    for (const auto& param : t.templateParams) {
        templ += typeToString(param) + ", ";
    }
    if (!t.templateParams.empty()) {
        templ.erase(templ.length() - 2);
        templ = "<" + templ + ">";
    }

    return t.base + templ;
}

} // namespace Cpp::Class
