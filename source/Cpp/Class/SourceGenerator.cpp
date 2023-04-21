#include "Cpp/Class/SourceGenerator.h"

#include <algorithm>
#include <ranges>

namespace Cpp::Class {

SourceGenerator::SourceGenerator(std::shared_ptr<Config> config)
    : m_config(std::move(config))
    , m_genUtils(m_config)
{
}

std::string SourceGenerator::generate(const Class& in)
{
    // there are a bunch of cases where we don't want to generate a source file
    if (in.isInterface) { // interfaces
        return "";
    }
    if (in.body.empty()) { // no body at all
        return "";
    }
    if (std::count_if(in.body.begin(),
                      in.body.end(),
                      [](const ClassElement& elem) { return std::holds_alternative<Method>(elem); }) == 0 &&
        std::count_if(in.body.begin(), in.body.end(), [](const ClassElement& elem) {
            return std::holds_alternative<Variable>(elem) && std::get<Variable>(elem).isStatic;
        }) == 0) { // no methods and no static members
        return "";
    }

    std::string ret;

    // include header
    ret += "#include \"";
    for (const auto& ns : in.namespaces) {
        ret += ns + "/";
    }
    ret += in.name + ".h\"\n\n";

    // open namespaces
    ret += m_genUtils.openNamespaces(in.namespaces);

    // static members
    for (const auto& elem : in.body) {
        if (std::holds_alternative<Variable>(elem) && std::get<Variable>(elem).isStatic) {
            const auto& var = std::get<Variable>(elem);
            ret += typeToString(var.type) + " " + in.name + "::" + var.name + ";\n";
        }
    }

    // methods
    for (const auto& elem : in.body) {
        if (std::holds_alternative<Method>(elem)) {
            const auto& m = std::get<Method>(elem);
            ret += typeToString(m.returnType) + " " + in.name + "::" + m.name + "(";
            if (!m.parameters.empty()) {
                for (const auto& param : m.parameters) {
                    ret += typeToString(param.type) + " " + param.name + ", ";
                }
                ret.erase(ret.length() - 2);
            }
            ret += ") {}\n\n";
        }
    }

    // close namespaces
    ret += m_genUtils.closeNamespaces(in.namespaces);

    return ret;
}

std::string SourceGenerator::typeToString(const Common::Type& t)
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
