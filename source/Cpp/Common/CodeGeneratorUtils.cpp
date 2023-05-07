#include "Cpp/Common/CodeGeneratorUtils.h"

#include <numeric>
#include <ranges>
#include <utility>

namespace Cpp::Common {

CodeGeneratorUtils::CodeGeneratorUtils(std::shared_ptr<Config> config)
    : m_config(std::move(config))
{
}

std::string CodeGeneratorUtils::openNamespaces(const std::list<std::string>& namespaces)
{
    std::string ret;
    if (m_config->concatenateNamespaces()) {
        if (!namespaces.empty()) {
            ret = "namespace ";
            ret += std::accumulate(++namespaces.begin(),
                                   namespaces.end(),
                                   namespaces.front(),
                                   [](const std::string& l, const std::string& r) { return l + "::" + r; });
            ret += " {\n";
        }
    } else {
        ret = std::accumulate(namespaces.begin(),
                              namespaces.end(),
                              std::string(),
                              [](const std::string& l, const std::string& r) { return l + "namespace " + r + " {\n"; });
    }
    return ret;
}

std::string CodeGeneratorUtils::closeNamespaces(const std::list<std::string>& namespaces)
{
    std::string ret;
    if (m_config->concatenateNamespaces()) {
        if (!namespaces.empty()) {
            ret = "} // namespace ";
            ret += std::accumulate(++namespaces.begin(),
                                   namespaces.end(),
                                   namespaces.front(),
                                   [](const std::string& l, const std::string& r) { return l + "::" + r; });
            ret += "\n";
        }
    } else {
        ret = std::accumulate(
            namespaces.rbegin(), namespaces.rend(), std::string(), [](const std::string& l, const std::string& r) {
                return l + "} // namespace " + r + "\n";
            });
    }
    return ret;
}

} // namespace Cpp::Common
