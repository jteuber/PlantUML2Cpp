#include "Cpp/SourceGenerator.h"

#include <ranges>

namespace Cpp {

std::string SourceGenerator::generate(const Class& in)
{
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

    // methods
    for (const auto& elem : in.body) {
        if (std::holds_alternative<Method>(elem)) {
            const Method& m = std::get<Method>(elem);
            std::string ret = typeToString(m.returnType) + " " + m.name + "(";
            if (!m.parameters.empty()) {
                for (const auto& param : m.parameters) {
                    ret += typeToString(param.type) + " " + param.name + ", ";
                }
                ret.erase(ret.length() - 2);
            }
        }
        ret += " {}\n\n";
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
