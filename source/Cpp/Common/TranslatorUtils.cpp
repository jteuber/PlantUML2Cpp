#include "Cpp/Common/TranslatorUtils.h"

#include <numeric>
#include <utility>

namespace Cpp::Common {

TranslatorUtils::TranslatorUtils(std::shared_ptr<Config> config)
    : m_config(std::move(config))
{}

Type TranslatorUtils::umlToCppType(PlantUml::Type umlType)
{
    Type out;

    out.base = umlType.base.back();
    for (auto ns : umlType.base | std::views::reverse | std::views::drop(1)) {
        out.base = ns + "::" + out.base;
    }

    auto it = m_config->umlToCppTypeMap.find(out.base);
    if (it != m_config->umlToCppTypeMap.end()) {
        out.base = it->second;
    }

    if (out.base.empty()) {
        out.base = "void";
    }

    for (auto& param : umlType.templateParams) {
        out.templateParams.push_back(umlToCppType(param));
    }

    return out;
}

Type TranslatorUtils::stringToCppType(std::string_view typeString)
{
    auto pos = typeString.find_first_of(",<>");
    Type ret{std::string(typeString.substr(0, pos))};

    if (pos != std::string_view::npos) {
        if (typeString[pos] == '<') {
            typeString.remove_prefix(pos + 1);
            ret.templateParams.push_back(stringToCppType(typeString));

            auto nextPos = typeString.find_first_of(",<>");
            while (typeString[nextPos] == ',') {
                typeString.remove_prefix(nextPos + 1);
                ret.templateParams.push_back(stringToCppType(typeString));
                nextPos = typeString.find_first_of(",<>");
            }
        }
    }

    return ret;
}

std::string TranslatorUtils::toNamespacedString(std::list<std::string> namespacedType)
{
    auto ret = std::accumulate(
        namespacedType.begin(), namespacedType.end(), std::string(), [](const auto& a, const auto& b) -> std::string {
            return a + (a.empty() ? "" : "::") + b;
        });
    if (namespacedType.front().empty()) {
        ret = "::" + ret;
    }

    return ret;
}

std::string TranslatorUtils::visibilityToString(PlantUml::Visibility vis)
{
    switch (vis) {
    case PlantUml::Visibility::Protected:
        return "protected:";
    case PlantUml::Visibility::Private:
        return "private:";
    case PlantUml::Visibility::Public:
        return "public:";
    default:
        return "";
    }
}

std::list<std::string> getEffectiveNamespace(std::list<std::string> umlTypename,
                                             const std::list<std::string>& namespaceStack)
{
    // not interested in the name
    umlTypename.pop_back();

    // uml typename starts with a dot => global namespace
    if (umlTypename.front().empty()) {
        umlTypename.pop_front();
    } else {
        for (const auto& ns : namespaceStack | std::views::reverse) {
            umlTypename.push_front(ns);
        }
    }

    return umlTypename;
}

} // namespace Cpp::Common