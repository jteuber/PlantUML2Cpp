#include "gtest/gtest.h"

#include <regex>

#include "Config.h"
#include "Cpp/Variant/HeaderGenerator.h"
#include "Cpp/Variant/Variant.h"

namespace Cpp {
namespace Variant {

static const std::string ws     = "( |\t|\n)*";
static const std::string header = "#pragma once" + ws + "#include <variant>" + ws;

TEST(VariantHeaderGenerator, Empty)
{
    // Arrange
    auto config = std::make_shared<Config>();
    HeaderGenerator sut(config);

    Variant input;
    input.name = "Empty";

    // Act
    auto output = sut.generate(input);

    // Assert
    std::string regex = header + "using[ \t]*Empty" + ws + "=" + ws + "std::variant<" + ws + ">;(.|\n)*";
    std::regex classRegex(regex);
    EXPECT_TRUE(std::regex_match(output, classRegex)) << output;
}

TEST(VariantHeaderGenerator, SingleType)
{
    // Arrange
    auto config = std::make_shared<Config>();
    HeaderGenerator sut(config);

    Variant input;
    input.name = "Simple";
    input.containedTypes.push_back(Common::Type{"Contained"});

    // Act
    auto output = sut.generate(input);

    // Assert
    std::string regex = header + R"(\#include \"Contained.h\")" + ws;
    regex += "using[ \t]*Simple" + ws + "=" + ws + R"(std::variant\<)" + ws + "Contained" + ws + R"(\>;(.|\n)*)";
    std::regex classRegex(regex);
    EXPECT_TRUE(std::regex_match(output, classRegex)) << output;
}

} // namespace Variant
} // namespace Cpp
