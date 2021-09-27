#pragma once

#include <algorithm>
#include <concepts>
#include <list>
#include <memory>
#include <string>
#include <vector>

#include "Config.h"
#include "Type.h"

#include "PlantUml/ModelElement.h"

namespace Cpp {

template <typename T>
concept NamespacedElement = requires(T a)
{
    {
        a.namespaces
    }
    ->std::convertible_to<std::list<std::string>>;
    {
        a.name
    }
    ->std::convertible_to<std::string>;
};

class TranslatorUtils
{
public:
    explicit TranslatorUtils(std::shared_ptr<Config> config);

    Type umlToCppType(PlantUml::Type umlType);
    Type stringToCppType(std::string_view typeString);
    std::string toNamespacedString(std::list<std::string> namespacedType);
    std::string visibilityToString(PlantUml::Visibility vis);

private:
    std::shared_ptr<Config> m_config;
};

std::list<std::string> getEffectiveNamespace(std::list<std::string> umlTypename,
                                             const std::list<std::string>& namespaceStack);

template <NamespacedElement E>
typename std::vector<E>::iterator findClass(const std::list<std::string>& umlTypename,
                                            std::vector<E>& classes,
                                            const std::list<std::string>& namespaceStack)
{
    return std::ranges::find_if(classes, [namespaceStack, subject = umlTypename](const E& c) {
        if (c.name == subject.back()) {
            auto subjectNamespace = getEffectiveNamespace(subject, namespaceStack);
            auto it               = subjectNamespace.begin();
            for (const auto& nc : c.namespaces) {
                if (it == subjectNamespace.end() || nc != *it) {
                    return false;
                }
                ++it;
            }
            return true;
        }
        return false;
    });
}

} // namespace Cpp
