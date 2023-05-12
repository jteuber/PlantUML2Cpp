#pragma once

#include <array>
#include <filesystem>
#include <string>
#include <unordered_map>

class Config
{
public:
    bool parseAndLoad(int argc, char** argv);

    const std::filesystem::path& projectPath() const;
    std::filesystem::path modelsPath() const;
    std::filesystem::path configPath() const;
    std::filesystem::path headersPath() const;
    std::filesystem::path sourcesPath() const;
    const std::string& headerFileExtention() const;
    const std::string& sourceFileExtention() const;

    const std::string& memberPrefix() const;
    const std::string& indent() const;
    bool noMemberPrefixForStructs() const;
    bool concatenateNamespaces() const;

    const std::unordered_map<std::string, std::string>& containerByCardinalityComposition() const;
    const std::unordered_map<std::string, std::string>& containerByCardinalityAggregation() const;

    const std::unordered_map<std::string, std::string>& typeToIncludeMap() const;
    const std::unordered_map<std::string, std::string>& umlToCppTypeMap() const;

private:
    // project and file generation settings
    std::filesystem::path m_projectPath = std::filesystem::current_path();
    std::string m_configFolderName      = "models";
    std::string m_modelFolderName       = "models";
    std::string m_includeFolderName     = "include";
    std::string m_sourceFolderName      = "source";
    std::string m_headerFileExtention   = ".h";
    std::string m_sourceFileExtention   = ".cpp";
    bool m_overwriteExistingFiles       = false;

    // code generation settings
    std::string m_memberPrefix    = "m_";
    std::string m_indent          = "    ";
    bool m_memberPrefixForStructs = false;
    bool m_concatenateNamespaces  = false;

    // translation settings
    std::unordered_map<std::string, std::string> m_containerByCardinalityComposition = {{"0..*", "std::vector<{}>"}};
    std::unordered_map<std::string, std::string> m_containerByCardinalityAggregation = {
        {"", "std::shared_ptr<{}>"}, {"1", "std::shared_ptr<{}>"}, {"0..*", "std::vector<std::shared_ptr<{}>>"}};

    std::unordered_map<std::string, std::string> m_typeToIncludeMap = {{"std::string", "string"},
                                                                       {"std::vector", "vector"},
                                                                       {"std::pair", "pair"},
                                                                       {"std::list", "list"},
                                                                       {"std::unordered_map", "unordered_map"},
                                                                       {"std::shared_ptr", "memory"},
                                                                       {"std::filesystem::path", "filesystem"}};

    std::unordered_map<std::string, std::string> m_umlToCppTypeMap = {{"string", "std::string"},
                                                                      {"vector", "std::vector"},
                                                                      {"pair", "std::pair"},
                                                                      {"list", "std::list"},
                                                                      {"umap", "std::unordered_map"},
                                                                      {"path", "std::filesystem::path"}};
};
