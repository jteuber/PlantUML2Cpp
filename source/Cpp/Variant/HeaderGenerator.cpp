#include "Cpp/Variant/HeaderGenerator.h"

#include <algorithm>
#include <numeric>

namespace Cpp::Variant {

std::string HeaderGenerator::generate(const Variant& in)
{
    std::string ret;

    // pragme once
    ret += "#pragma once\n\n";

    auto incs =
        in.containedTypes | std::views::transform([](const Type& inc) { return "#include \"" + inc.base + "\"\n"; });
    ret += std::accumulate(incs.begin(), incs.end(), std::string());

    // open namespaces
    for (const auto& ns : in.namespaces) {
        ret += "namespace " + ns + " {\n";
    }
    ret += "\n";

    // variant definition
    ret += "\nusing " + in.name + " = std::variant<";

    auto types = in.containedTypes | std::views::transform([](const Type& inc) { return inc.base + ", "; });
    ret += std::accumulate(types.begin(), types.end(), std::string());
    ret = ret.substr(0, ret.size() - 2);

    ret += ">;";

    // close namespaces
    for (const auto& ns : in.namespaces | std::views::reverse) {
        ret += "} // namespace " + ns + "\n";
    }

    return ret;
}

} // namespace Cpp::Variant
