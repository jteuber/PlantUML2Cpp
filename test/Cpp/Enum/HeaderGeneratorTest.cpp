#include "gtest/gtest.h"

#include <regex>

#include "Config.h"
#include "Cpp/Enum/Enum.h"
#include "Cpp/Enum/HeaderGenerator.h"

namespace Cpp::Enum {

static const std::string ws     = "( |\t|\n)*";
static const std::string header = "#pragma once" + ws;

TEST(EnumHeaderGenerator, Empty)
{
    // Arrange
    auto config = std::make_shared<Config>();
    HeaderGenerator sut(config);

    Enum input;
    input.name = "Empty";

    // Act
    auto output = sut.generate(input);

    // Assert
    std::string regex = header + "enum class[ \t]*Empty" + ws + R"(\{)" + ws + R"(\};(.|\n)*)";
    std::regex classRegex(regex);
    EXPECT_TRUE(std::regex_match(output, classRegex)) << output;
}

TEST(EnumHeaderGenerator, SingleEnumerator)
{
    // Arrange
    auto config = std::make_shared<Config>();
    HeaderGenerator sut(config);

    Enum input;
    input.name = "Simple";
    input.enumerators.push_back(Enumerator{"Contained"});

    // Act
    auto output = sut.generate(input);

    // Assert
    std::string regex = header;
    regex += "enum class[ \t]*Simple" + ws + R"(\{)" + ws + "Contained" + ws + R"(\};(.|\n)*)";
    std::regex classRegex(regex);
    EXPECT_TRUE(std::regex_match(output, classRegex)) << output;
}

TEST(EnumHeaderGenerator, MultipleEnumerators)
{
    // Arrange
    auto config = std::make_shared<Config>();
    HeaderGenerator sut(config);

    Enum input;
    input.name = "Simple";
    input.enumerators.push_back(Enumerator{"Contained1"});
    input.enumerators.push_back(Enumerator{"Contained2"});

    // Act
    auto output = sut.generate(input);

    // Assert
    std::string regex = header;
    regex += "enum class[ \t]*Simple" + ws + R"(\{)" + ws + "Contained1," + ws + "Contained2" + ws + R"(\};(.|\n)*)";
    std::regex classRegex(regex);
    EXPECT_TRUE(std::regex_match(output, classRegex)) << output;
}

} // namespace Cpp::Enum
