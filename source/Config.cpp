#include "Config.h"

#include "CLI/App.hpp"
#include "CLI/Config.hpp"
#include "CLI/Formatter.hpp"

bool Config::parseAndLoad(int argc, char** argv)
{
    CLI::App app{"App description"};

    std::string pathString = m_projectPath.string();
    app.add_option("path", pathString, "Path to project directory");

    try {
        app.parse((argc), (argv));
    } catch (const CLI::ParseError& e) {
        app.exit(e);
        return false;
    }

    m_projectPath = pathString;

    return true;
}

const std::filesystem::path& Config::projectPath() const
{
    return m_projectPath;
}
std::filesystem::path Config::modelsPath() const
{
    return m_projectPath / m_modelFolderName;
}
std::filesystem::path Config::configPath() const
{
    return m_projectPath / m_configFolderName / "config.json";
}
std::filesystem::path Config::headersPath() const
{
    return m_projectPath / m_includeFolderName;
}
std::filesystem::path Config::sourcesPath() const
{
    return m_projectPath / m_sourceFolderName;
}
const std::string& Config::headerFileExtention() const
{
    return m_headerFileExtention;
}
const std::string& Config::sourceFileExtention() const
{
    return m_sourceFileExtention;
}

const std::string& Config::memberPrefix() const
{
    return m_memberPrefix;
}
const std::string& Config::indent() const
{
    return m_indent;
}
bool Config::noMemberPrefixForStructs() const
{
    return m_noMemberPrefixForStructs;
}

const std::unordered_map<std::string, std::string>& Config::containerByCardinalityComposition() const
{
    return m_containerByCardinalityComposition;
}
const std::unordered_map<std::string, std::string>& Config::containerByCardinalityAggregation() const
{
    return m_containerByCardinalityAggregation;
}

const std::unordered_map<std::string, std::string>& Config::typeToIncludeMap() const
{
    return m_typeToIncludeMap;
}
const std::unordered_map<std::string, std::string>& Config::umlToCppTypeMap() const
{
    return m_umlToCppTypeMap;
}

bool Config::concatenateNamespaces() const
{
    return m_concatenateNamespaces;
}