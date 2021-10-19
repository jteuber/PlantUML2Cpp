#include "Cpp/Variant/HeaderGenerator.h"

#include <algorithm>
#include <numeric>

namespace Cpp::Variant {

HeaderGenerator::HeaderGenerator(std::shared_ptr<Config> config)
    : m_config(std::move(config))
    , m_genUtils(m_config)
{}

std::string HeaderGenerator::generate(const Variant& in)
{
    std::string ret;

    // pragme once
    ret += "#pragma once\n\n";

    auto incs = in.containedTypes |
                std::views::transform([](const Common::Type& inc) { return "#include \"" + inc.base + ".h\"\n"; });
    ret += std::accumulate(incs.begin(), incs.end(), std::string());

    // open namespaces
    ret += m_genUtils.openNamespaces(in.namespaces);

    // variant definition
    ret += "using " + in.name + " = std::variant<";

    if (!in.containedTypes.empty()) {
        ret += std::accumulate(++in.containedTypes.begin(),
                               in.containedTypes.end(),
                               in.containedTypes.front().base,
                               [](const std::string& l, const Common::Type& r) { return l + ", " + r.base; });
    }

    ret += ">;";

    // close namespaces
    ret += m_genUtils.closeNamespaces(in.namespaces);

    return ret;
}

} // namespace Cpp::Variant
