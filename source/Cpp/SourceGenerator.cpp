#include "Cpp/SourceGenerator.h"

#include <ranges>

namespace Cpp {

std::string SourceGenerator::generate(const Class& in)
{
    // there are a bunch of cases where we don't want to generate a source file
    if (in.isInterface) { // interfaces
        return "";
    } else if (in.body.empty()) { // no body at all
        return "";
    } else if (std::count_if(in.body.begin(),
                             in.body.end(),
                             [](const BodyElement& elem) { return std::holds_alternative<Method>(elem); }) == 0 &&
               std::count_if(in.body.begin(), in.body.end(), [](const BodyElement& elem) {
                   return std::holds_alternative<Variable>(elem) && std::get<Variable>(elem).isStatic;
               }) == 0) { // no methods and no static members
        return "";
    }

    std::string ret;

    // include header
    ret += "#include \"";
    for (const auto& ns : in.namespaces)
        ret += ns + "/";
    ret += in.name + ".h\"\n\n";

    // open namespaces
    for (const auto& ns : in.namespaces) {
        ret += "namespace " + ns + " {\n";
    }
    ret += "\n";

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
    for (const auto& ns : in.namespaces | std::views::reverse) {
        ret += "} // namespace " + ns + "\n";
    }

    return ret;
}

std::string SourceGenerator::typeToString(const Type& t)
{
    std::string templ;
    for (const auto& param : t.templateParams)
        templ += typeToString(param) + ", ";
    if (!t.templateParams.empty()) {
        templ.erase(templ.length() - 2);
        templ = "<" + templ + ">";
    }

    return t.base + templ;
}

} // namespace Cpp
