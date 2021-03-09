#pragma once

#include "Class.h"

#include <array>
#include <string>
#include <unordered_map>

struct Config
{
    std::string memberPrefix      = "m_";
    std::string indent            = "    ";
    bool noMemberPrefixForStructs = true;

    std::unordered_map<std::string, std::string> containerByCardinalityComposition = {
        {"", "std::shared_ptr<{}>"}, {"1", "std::shared_ptr<{}>"}, {"0..*", "std::vector<{}>"}};
    std::unordered_map<std::string, std::string> containerByCardinalityAggregation = {
        {"", "std::shared_ptr<{}>"}, {"1", "std::shared_ptr<{}>"}, {"0..*", "std::vector<std::shared_ptr<{}>>"}};

    std::array<std::pair<Visibility, std::string>, 8> memberOrder = {std::pair{Visibility::Unspecified, "methods"},
                                                                     std::pair{Visibility::Unspecified, "variables"},
                                                                     std::pair{Visibility::Public, "methods"},
                                                                     std::pair{Visibility::Public, "variables"},
                                                                     std::pair{Visibility::Protected, "methods"},
                                                                     std::pair{Visibility::Protected, "variables"},
                                                                     std::pair{Visibility::Private, "methods"},
                                                                     std::pair{Visibility::Private, "variables"}};

    std::unordered_map<std::string, std::string> typeToIncludeMap = {{"std::shared_ptr", "memory"},
                                                                     {"std::filesystem::path", "filesystem"}};
};