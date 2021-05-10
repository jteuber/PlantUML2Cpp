#include "gtest/gtest.h"

#include <regex>

#include "Config.h"
#include "Cpp/Class.h"
#include "Cpp/SourceGenerator.h"

namespace Cpp {

static const std::string ws     = "( |\t|\n)*";
static const std::string header = "#include \"test.h\"" + ws;

// empty classes should not generate a source file
TEST(SourceGenerator, EmptyClass)
{
    // Arrange
    SourceGenerator sut;

    Class input;
    input.name = "test";

    // Act
    auto output = sut.generate(input);

    // Assert
    EXPECT_TRUE(output.empty()) << output;
}

TEST(SourceGenerator, NoMethodStruct)
{
    // Arrange
    SourceGenerator sut;

    Class input;
    input.name     = "test";
    input.isStruct = true;
    input.body.emplace_back(Variable{"var", Type{"int"}});

    // Act
    auto output = sut.generate(input);

    // Assert
    EXPECT_TRUE(output.empty()) << output;
}

TEST(SourceGenerator, Interface)
{
    // Arrange
    SourceGenerator sut;

    Class input;
    input.name        = "test";
    input.isInterface = true;
    input.body.emplace_back(Method{"method", Type{"int"}});

    // Act
    auto output = sut.generate(input);

    // Assert
    EXPECT_TRUE(output.empty()) << output;
}

TEST(SourceGenerator, SingleMethodClass)
{
    // Arrange
    SourceGenerator sut;

    Class input;
    input.name = "test";
    input.body.emplace_back(Method{"method", Type{"int"}});

    // Act
    auto output = sut.generate(input);

    // Assert
    std::string regex = header + "int test::method\\(\\) \\{\\}" + ws;
    std::regex classRegex(regex);
    EXPECT_TRUE(std::regex_match(output, classRegex)) << output;
}

TEST(SourceGenerator, SingleMethodStruct)
{
    // Arrange
    SourceGenerator sut;

    Class input;
    input.name     = "test";
    input.isStruct = true;
    input.body.emplace_back(Method{"method", Type{"int"}});

    // Act
    auto output = sut.generate(input);

    // Assert
    std::string regex = header + "int test::method\\(\\) \\{\\}" + ws;
    std::regex classRegex(regex);
    EXPECT_TRUE(std::regex_match(output, classRegex)) << output;
}

TEST(SourceGenerator, ComplexMethodClass)
{
    // Arrange
    SourceGenerator sut;

    Class input;
    input.name = "test";

    Method method;
    method.name       = "complexMethod";
    method.returnType = Type{"std::vector", {Type{"std::pair", {Type{"Visibility"}, Type{"std::string"}}}}};
    method.parameters = {{"param1", Type{"std::vector", {Type{"int"}}}},
                         {"param2", Type{"std::pair", {Type{"Visibility"}, Type{"std::string"}}}}};
    input.body.push_back(method);

    // Act
    auto output = sut.generate(input);

    // Assert
    std::string regex = header + "std::vector<std::pair<Visibility, std::string>>" + ws;
    regex += "test::complexMethod\\(" + ws;
    regex += "std::vector<int>" + ws + "param1," + ws;
    regex += "std::pair<Visibility, std::string>" + ws + "param2" + ws;
    regex += "\\)" + ws + "\\{\\}" + ws;
    std::regex classRegex(regex);
    EXPECT_TRUE(std::regex_match(output, classRegex)) << output;
}

TEST(SourceGenerator, MultiMethodClass)
{
    // Arrange
    SourceGenerator sut;

    Class input;
    input.name = "test";

    Method method;
    method.name       = "complexMethod";
    method.returnType = Type{"std::vector", {Type{"std::pair", {Type{"Visibility"}, Type{"std::string"}}}}};
    method.parameters = {{"param1", Type{"std::vector", {Type{"int"}}}},
                         {"param2", Type{"std::pair", {Type{"Visibility"}, Type{"std::string"}}}}};
    input.body.push_back(method);
    input.body.emplace_back(Method{"method", Type{"int"}});

    // Act
    auto output = sut.generate(input);

    // Assert
    std::string regex = header + "std::vector<std::pair<Visibility, std::string>>" + ws;
    regex += "test::complexMethod\\(" + ws;
    regex += "std::vector<int>" + ws + "param1," + ws;
    regex += "std::pair<Visibility, std::string>" + ws + "param2" + ws;
    regex += "\\)" + ws + "\\{\\}" + ws;
    regex += "int test::method\\(\\) \\{\\}" + ws;
    std::regex classRegex(regex);
    EXPECT_TRUE(std::regex_match(output, classRegex)) << output;
}

} // namespace Cpp
