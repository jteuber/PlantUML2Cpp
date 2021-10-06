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
        in.containedTypes | std::views::transform([](const Type& inc) { return "#include \"" + inc.base + ".h\"\n"; });
    ret += std::accumulate(incs.begin(), incs.end(), std::string());

    // open namespaces
    for (const auto& ns : in.namespaces) {
        ret += "namespace " + ns + " {\n";
    }
    ret += "\n";

    // variant definition
    ret += "using " + in.name + " = std::variant<";

    if (!in.containedTypes.empty()) {
        auto types = in.containedTypes | std::views::transform([](const Type& inc) { return inc.base; });
        ret += std::accumulate(++types.begin(),
                               types.end(),
                               types.front(),
                               [](const std::string& l, const std::string& r) { return l + ", " + r; });
    }

    ret += ">;";

    // close namespaces
    for (const auto& ns : in.namespaces | std::views::reverse) {
        ret += "} // namespace " + ns + "\n";
    }

    return ret;
}

} // namespace Cpp::Variant
