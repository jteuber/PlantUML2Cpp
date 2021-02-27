#pragma once

#include <array>
#include <string>
#include <unordered_map>

struct Config
{
    std::string memberPrefix = "m_";
    std::string indent       = "    ";

    std::unordered_map<std::string, std::string> containerByCardinalityComposition = {
        {"", "std::shared_ptr<{}>"}, {"1", "std::shared_ptr<{}>"}, {"0..*", "std::vector<{}>"}};
    std::unordered_map<std::string, std::string> containerByCardinalityAggregation = {
        {"", "std::shared_ptr<{}>"}, {"1", "std::shared_ptr<{}>"}, {"0..*", "std::vector<std::shared_ptr<{}>>"}};

    std::array<std::string, 6> memberOrder{"public_methods",
                                           "public_members",
                                           "protected_methods",
                                           "protected_members",
                                           "private_methods",
                                           "private_members"};
};