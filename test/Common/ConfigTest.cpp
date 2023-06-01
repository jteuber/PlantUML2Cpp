#include "gtest/gtest.h"

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