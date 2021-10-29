#include "Cpp/Enum/HeaderGenerator.h"

#include <algorithm>
#include <numeric>

namespace Cpp::Enum {
HeaderGenerator::HeaderGenerator(std::shared_ptr<Config> config)
    : m_config(std::move(config))
    , m_genUtils(m_config)
{}

std::string HeaderGenerator::generate(const Enum& in)
{
    std::string ret;

    // pragme once
    ret += "#pragma once\n\n";

    // open namespaces
    ret += m_genUtils.openNamespaces(in.namespaces);

    // variant definition
    ret += "enum class " + in.name + "{\n";

    if (!in.enumerators.empty()) {
        ret += std::accumulate(
            ++in.enumerators.begin(),
            in.enumerators.end(),
            m_config->indent + in.enumerators.front().name,
            [this](const std::string& l, const Enumerator& r) { return l + ",/n" + m_config->indent + r.name; });
    }

    ret += "\n};";

    // close namespaces
    ret += m_genUtils.closeNamespaces(in.namespaces);

    return ret;
}

} // namespace Cpp::Enum
