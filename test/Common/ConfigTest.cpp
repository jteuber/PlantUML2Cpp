#include "gtest/gtest.h"

#include <filesystem>
#include <fstream>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include "Config.h"

TEST(ConfigTest, parseBasicArgs)
{
    // Arrange
    Config sut{};

    std::vector<std::string> arguments = {"test", "-c", "config", "path/to/project"};
    std::vector<char*> argv;
    for (const auto& arg : arguments)
        argv.push_back((char*) arg.data());
    argv.push_back(nullptr);

    // Act
    sut.parseAndLoad(argv.size() - 1, argv.data());

    // Assert
    EXPECT_EQ(sut.projectPath(), "path/to/project");
    EXPECT_EQ(sut.configPath(), "path/to/project/config/config.json");
}

TEST(ConfigTest, parseAllArgs)
{
    // Arrange
    Config sut{};

    std::vector<std::string> arguments = {
        "test", "-c", "config", "-fMn", "-mmo", "-iin", "-ssrc", "-Hhpp", "-Ccp", "-ppre", "-tt", "path/to/project"};
    std::vector<char*> argv;
    for (const auto& arg : arguments)
        argv.push_back((char*) arg.data());
    argv.push_back(nullptr);

    // Act
    sut.parseAndLoad(argv.size() - 1, argv.data());

    // Assert
    EXPECT_EQ(sut.projectPath(), "path/to/project");
    EXPECT_EQ(sut.configPath(), "path/to/project/config/config.json");
    EXPECT_TRUE(sut.overwriteExistingFiles());
    EXPECT_FALSE(sut.noMemberPrefixForStructs());
    EXPECT_TRUE(sut.concatenateNamespaces());
    EXPECT_EQ(sut.modelsPath(), "path/to/project/mo");
    EXPECT_EQ(sut.headersPath(), "path/to/project/in");
    EXPECT_EQ(sut.sourcesPath(), "path/to/project/src");
    EXPECT_EQ(sut.headerFileExtention(), "hpp");
    EXPECT_EQ(sut.sourceFileExtention(), "cp");
    EXPECT_EQ(sut.memberPrefix(), "pre");
    EXPECT_EQ(sut.indent(), "t");
}

TEST(ConfigTest, configFileReading)
{
    // Arrange
    Config sut{};

    std::filesystem::path thisFile(__FILE__);
    auto testDir = thisFile.parent_path().parent_path();

    std::vector<std::string> arguments = {"test", "-c", "Common", testDir};
    std::vector<char*> argv;
    for (const auto& arg : arguments)
        argv.push_back((char*) arg.data());
    argv.push_back(nullptr);

    // Act
    sut.parseAndLoad(argv.size() - 1, argv.data());

    // Assert
    EXPECT_TRUE(sut.overwriteExistingFiles());
    EXPECT_FALSE(sut.noMemberPrefixForStructs());
    EXPECT_TRUE(sut.concatenateNamespaces());
    EXPECT_EQ(sut.modelsPath(), testDir / "mo");
    EXPECT_EQ(sut.headersPath(), testDir / "in");
    EXPECT_EQ(sut.sourcesPath(), testDir / "src");
    EXPECT_EQ(sut.headerFileExtention(), "hpp");
    EXPECT_EQ(sut.sourceFileExtention(), "cp");
    EXPECT_EQ(sut.memberPrefix(), "pre");
    EXPECT_EQ(sut.indent(), "t");
    EXPECT_EQ(sut.containerByCardinalityComposition().size(), 1);
    EXPECT_EQ(sut.containerByCardinalityComposition().at("test"), "test");
    EXPECT_EQ(sut.containerByCardinalityAggregation().size(), 1);
    EXPECT_EQ(sut.containerByCardinalityAggregation().at("test"), "test");
    EXPECT_EQ(sut.typeToIncludeMap().size(), 1);
    EXPECT_EQ(sut.typeToIncludeMap().at("test"), "test");
    EXPECT_EQ(sut.umlToCppTypeMap().size(), 1);
    EXPECT_EQ(sut.umlToCppTypeMap().at("test"), "test");
}

TEST(ConfigTest, configFileWriting)
{
    // Arrange
    Config sut{};

    std::filesystem::path thisFile(__FILE__);
    auto testDir = thisFile.parent_path().parent_path();
    std::filesystem::create_directory(testDir / "tmp");

    std::vector<std::string> arguments = {
        "test", "-c", "tmp", "-fMnw", "-mmo", "-iin", "-ssrc", "-Hhpp", "-Ccp", "-ppre", "-tt", testDir};
    std::vector<char*> argv;
    for (const auto& arg : arguments)
        argv.push_back((char*) arg.data());
    argv.push_back(nullptr);

    // Act
    sut.parseAndLoad(argv.size() - 1, argv.data());

    // Assert
    std::ifstream genFile(testDir / "tmp" / "config.json");
    json generatedJson;
    genFile >> generatedJson;

    EXPECT_TRUE(generatedJson["overwriteExistingFiles"]);
    EXPECT_TRUE(generatedJson["memberPrefixForStructs"]);
    EXPECT_TRUE(generatedJson["concatenateNamespaces"]);
    EXPECT_EQ(generatedJson["modelFolderName"], "mo");
    EXPECT_EQ(generatedJson["includeFolderName"], "in");
    EXPECT_EQ(generatedJson["sourceFolderName"], "src");
    EXPECT_EQ(generatedJson["headerFileExtention"], "hpp");
    EXPECT_EQ(generatedJson["sourceFileExtention"], "cp");
    EXPECT_EQ(generatedJson["memberPrefix"], "pre");
    EXPECT_EQ(generatedJson["indent"], "t");
    genFile.close();

    std::filesystem::remove(testDir / "tmp" / "config.json");
    std::filesystem::remove(testDir / "tmp");
}