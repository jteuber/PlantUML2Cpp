#pragma once

#include "Cpp/Class/Class.h"

#include <array>
#include <string>
#include <unordered_map>

struct Config
{
    std::string includeFolderName = "include";
    std::string sourceFolderName  = "source";

    std::string memberPrefix      = "m_";
    std::string indent            = "    ";
    bool noMemberPrefixForStructs = true;

    std::unordered_map<std::string, std::string> containerByCardinalityComposition = {{"0..*", "std::vector<{}>"}};
    std::unordered_map<std::string, std::string> containerByCardinalityAggregation = {
        {"", "std::shared_ptr<{}>"}, {"1", "std::shared_ptr<{}>"}, {"0..*", "std::vector<std::shared_ptr<{}>>"}};

    std::unordered_map<std::string, std::string> typeToIncludeMap = {{"std::string", "string"},
                                                                     {"std::vector", "vector"},
                                                                     {"std::pair", "pair"},
                                                                     {"std::list", "list"},
                                                                     {"std::unordered_map", "unordered_map"},
                                                                     {"std::shared_ptr", "memory"},
                                                                     {"std::filesystem::path", "filesystem"}};

    std::unordered_map<std::string, std::string> umlToCppTypeMap = {{"string", "std::string"},
                                                                    {"vector", "std::vector"},
                                                                    {"pair", "std::pair"},
                                                                    {"list", "std::list"},
                                                                    {"umap", "std::unordered_map"}};
};
