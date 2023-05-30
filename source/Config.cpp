#include "Config.h"

#include "CLI/App.hpp"
#include "CLI/Config.hpp"
#include "CLI/Formatter.hpp"

bool Config::parseAndLoad(int argc, char** argv)
{
    CLI::App app{"PlantUML2Cpp -- translate PlantUML class diagrams to C++ code"};

    // Parse the command line arguments for only the config location
    std::string pathString = m_projectPath.string();
    app.add_option("path", pathString, "Path to project directory (default: current path)");
    app.add_option(
        "-c,--config",
        m_configFolderName,
        "Path to the folder containing the config.json, relative to project directory (default: \"models\")");

    try {
        app.parse((argc), (argv));
    } catch (const CLI::ParseError& e) {
        app.exit(e);
        return false;
    }

    m_projectPath = pathString;

    // TODO: read config.json from configPath()

    // now parse the rest of the command line arguments
    app.add_flag("-f", m_overwriteExistingFiles, "Overwrite existing files when generating code");

    app.add_option("-m,--models", m_modelFolderName, "Folder containing the PlantUML files (default: \"models\")");
    app.add_option(
        "-i,--includeDir", m_includeFolderName, "Folder to generate the header files into (default: \"include\")");
    app.add_option(
        "-s,--sourceDir", m_sourceFolderName, "Folder to generate the source files into (default: \"source\")");
    app.add_option(
        "-h,--headerExt", m_headerFileExtention, "Extension to use for generating header files (default: \".h\")");
    app.add_option(
        "-c,--sourceExt", m_sourceFileExtention, "Extension to use for generating source files (default: \".cpp\")");

    app.add_option("-p,--memberPrefix", m_memberPrefix, "Prefix used for member variables in classes(default: \"m_\")");
    app.add_option("-t,--indent", m_indent, "String to use as indent (default: \"    \" (4 spaces))");

    app.add_flag("-m,--memberPrefixForStruct", m_memberPrefixForStructs, "Use the member prefix also in structs");
    app.add_flag("-n,--concatenateNamespaces", m_concatenateNamespaces, "Use C++17's nested namespaces");
    bool writeConfig = false;
    app.add_flag(
        "-C,--writeConfig", writeConfig, "Write config file to config directory with the settings given as arguments");

    try {
        app.parse((argc), (argv));
    } catch (const CLI::ParseError& e) {
        app.exit(e);
        return false;
    }

    if (writeConfig) {
        // TODO: write config to configPath()
    }

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
    return !m_memberPrefixForStructs;
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