#include "Config.h"

#include "CLI/App.hpp"
#include "CLI/Config.hpp"
#include "CLI/Formatter.hpp"

#include <nlohmann/json.hpp>
using json = nlohmann::json;

bool Config::parseAndLoad(int argc, char** argv)
{
    CLI::App app{"PlantUML2Cpp -- translate PlantUML class diagrams to C++ code"};

    // Set up the command line options and flags
    std::string pathString = m_projectPath.string();
    app.add_option("path", pathString, "Path to project directory (default: current path)");
    app.add_option(
        "-c,--config",
        m_configFolderName,
        "Path to the folder containing the config.json, relative to project directory (default: \"models\")");

    app.add_flag("-f", m_overwriteExistingFiles, "Overwrite existing files when generating code");

    app.add_option("-m,--models", m_modelFolderName, "Folder containing the PlantUML files (default: \"models\")");
    app.add_option(
        "-i,--includeDir", m_includeFolderName, "Folder to generate the header files into (default: \"include\")");
    app.add_option(
        "-s,--sourceDir", m_sourceFolderName, "Folder to generate the source files into (default: \"source\")");
    app.add_option(
        "-H,--headerExt", m_headerFileExtention, "Extension to use for generating header files (default: \"h\")");
    app.add_option(
        "-C,--sourceExt", m_sourceFileExtention, "Extension to use for generating source files (default: \"cpp\")");

    app.add_option("-p,--memberPrefix", m_memberPrefix, "Prefix used for member variables in classes(default: \"m_\")");
    app.add_option("-t,--indent", m_indent, "String to use as indent (default: \"    \" (4 spaces))");

    app.add_flag("-M,--memberPrefixForStruct", m_memberPrefixForStructs, "Use the member prefix also in structs");
    app.add_flag("-n,--concatenateNamespaces", m_concatenateNamespaces, "Use C++17's nested namespaces");

    bool writeConfig = false;
    app.add_flag(
        "-w,--writeConfig", writeConfig, "Write config file to config directory with the settings given as arguments");

    // first parse the command line args to extract the project and config path
    // (all options and flags need to be present in case of -h or parser exception)
    try {
        app.parse((argc), (argv));
    } catch (const CLI::ParseError& e) {
        app.exit(e);
        return false;
    }

    m_projectPath = pathString;

    // read config.json from configPath()
    readConfigFrom(configPath());

    // now parse the command line arguments again to overwrite anything set by the config
    try {
        app.parse((argc), (argv));
    } catch (const CLI::ParseError& e) {
        app.exit(e);
        return false;
    }

    // write config to configPath() if requested
    if (writeConfig) {
        writeConfigTo(configPath());
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

bool Config::overwriteExistingFiles() const
{
    return m_overwriteExistingFiles;
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

bool Config::concatenateNamespaces() const
{
    return m_concatenateNamespaces;
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

void Config::readConfigFrom(std::filesystem::path configFilePath)
{
    std::ifstream i(configFilePath);
    if (i.fail())
        return;

    json config;
    i >> config;
    if (!config.is_object())
        return;

    if (config.contains("modelFolderName"))
        m_modelFolderName = config["modelFolderName"].get<std::string>();

    if (config.contains("includeFolderName"))
        m_includeFolderName = config["includeFolderName"].get<std::string>();

    if (config.contains("sourceFolderName"))
        m_sourceFolderName = config["sourceFolderName"].get<std::string>();

    if (config.contains("headerFileExtention"))
        m_headerFileExtention = config["headerFileExtention"].get<std::string>();

    if (config.contains("sourceFileExtention"))
        m_sourceFileExtention = config["sourceFileExtention"].get<std::string>();

    if (config.contains("overwriteExistingFiles"))
        m_overwriteExistingFiles = config["overwriteExistingFiles"].get<bool>();

    if (config.contains("memberPrefix"))
        m_memberPrefix = config["memberPrefix"].get<std::string>();

    if (config.contains("indent"))
        m_indent = config["indent"].get<std::string>();

    if (config.contains("memberPrefixForStructs"))
        m_memberPrefixForStructs = config["memberPrefixForStructs"].get<bool>();

    if (config.contains("concatenateNamespaces"))
        m_concatenateNamespaces = config["concatenateNamespaces"].get<bool>();

    if (config.contains("containerByCardinalityComposition"))
        m_containerByCardinalityComposition =
            config["containerByCardinalityComposition"].get<std::unordered_map<std::string, std::string>>();

    if (config.contains("containerByCardinalityAggregation"))
        m_containerByCardinalityAggregation =
            config["containerByCardinalityAggregation"].get<std::unordered_map<std::string, std::string>>();

    if (config.contains("typeToIncludeMap"))
        m_typeToIncludeMap = config["typeToIncludeMap"].get<std::unordered_map<std::string, std::string>>();

    if (config.contains("umlToCppTypeMap"))
        m_umlToCppTypeMap = config["umlToCppTypeMap"].get<std::unordered_map<std::string, std::string>>();
}

void Config::writeConfigTo(std::filesystem::path configFilePath)
{
    json config;

    config["modelFolderName"]        = m_modelFolderName;
    config["includeFolderName"]      = m_includeFolderName;
    config["sourceFolderName"]       = m_sourceFolderName;
    config["headerFileExtention"]    = m_headerFileExtention;
    config["sourceFileExtention"]    = m_sourceFileExtention;
    config["overwriteExistingFiles"] = m_overwriteExistingFiles;

    config["memberPrefix"] = m_memberPrefix;
    config["indent"]       = m_indent;

    config["memberPrefixForStructs"] = m_memberPrefixForStructs;
    config["concatenateNamespaces"]  = m_concatenateNamespaces;

    config["containerByCardinalityComposition"] = m_containerByCardinalityComposition;
    config["containerByCardinalityAggregation"] = m_containerByCardinalityAggregation;
    config["typeToIncludeMap"]                  = m_typeToIncludeMap;
    config["umlToCppTypeMap"]                   = m_umlToCppTypeMap;

    std::ofstream o(configFilePath);
    o << std::setw(4) << config << std::endl;
    o.close();
}
