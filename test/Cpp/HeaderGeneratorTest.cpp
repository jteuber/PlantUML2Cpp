#include "gtest/gtest.h"

#include <regex>

#include "Config.h"
#include "Cpp/Class.h"
#include "Cpp/HeaderGenerator.h"

namespace Cpp {

static const std::string ws     = "( |\t|\n)*";
static const std::string header = "#pragma once" + ws;

TEST(HeaderGenerator, EmptyClass)
{
    // Arrange
    auto config = std::make_shared<Config>();
    HeaderGenerator sut(config);

    Class input;
    input.name = "empty";

    // Act
    auto output = sut.generate(input);

    // Assert
    std::string regex = header + "class[ \t]*empty" + ws + "\\{" + ws + "\\};(.|\n)*";
    std::regex classRegex(regex);
    EXPECT_TRUE(std::regex_match(output, classRegex)) << output;
}

TEST(HeaderGenerator, SinglePublicMethod)
{
    // Arrange
    auto config = std::make_shared<Config>();
    HeaderGenerator sut(config);

    Method method;
    method.name       = "simpleMethod";
    method.returnType = Type{"void"};

    Class input;
    input.name = "simpleClass";
    input.body.push_back(VisibilityKeyword{"public:"});
    input.body.push_back(method);

    // Act
    auto output = sut.generate(input);

    // Assert
    std::string regex = header + "class[ \t]*simpleClass" + ws + "\\{" + ws + "public:" + ws + "void" + ws +
                        "simpleMethod\\(\\);" + ws + "\\};(.|\n)*";
    std::regex classRegex(regex);
    EXPECT_TRUE(std::regex_match(output, classRegex)) << output;
}

TEST(HeaderGenerator, SinglePrivateMethod)
{
    // Arrange
    auto config = std::make_shared<Config>();
    HeaderGenerator sut(config);

    Method method;
    method.name       = "simpleMethod";
    method.returnType = Type{"void"};

    Class input;
    input.name = "simpleClass";
    input.body.push_back(VisibilityKeyword{"private:"});
    input.body.push_back(method);

    // Act
    auto output = sut.generate(input);

    // Assert
    std::string regex = header + "class[ \t]*simpleClass" + ws + "\\{" + ws + "private:" + ws + "void" + ws +
                        "simpleMethod\\(\\);" + ws + "\\};(.|\n)*";
    std::regex classRegex(regex);
    EXPECT_TRUE(std::regex_match(output, classRegex)) << output;
}

TEST(HeaderGenerator, SingleProtectedMethod)
{
    // Arrange
    auto config = std::make_shared<Config>();
    HeaderGenerator sut(config);

    Method method;
    method.name       = "simpleMethod";
    method.returnType = Type{"void"};

    Class input;
    input.name = "simpleClass";
    input.body.push_back(VisibilityKeyword{"protected:"});
    input.body.push_back(method);

    // Act
    auto output = sut.generate(input);

    // Assert
    std::string regex = header + "class[ \t]*simpleClass" + ws + "\\{" + ws + "protected:" + ws + "void" + ws +
                        "simpleMethod\\(\\);" + ws + "\\};(.|\n)*";
    std::regex classRegex(regex);
    EXPECT_TRUE(std::regex_match(output, classRegex)) << output;
}

TEST(HeaderGenerator, AllKindsOfMethods)
{
    // Arrange
    auto config = std::make_shared<Config>();
    HeaderGenerator sut(config);

    Method method;
    method.name       = "simplePublicMethod";
    method.returnType = Type{"void"};

    Class input;
    input.name = "simpleClass";
    input.body.push_back(VisibilityKeyword{"public:"});
    input.body.push_back(method);

    method.name = "simpleProtectedMethod";
    input.body.push_back(VisibilityKeyword{"protected:"});
    input.body.push_back(method);

    method.name = "simplePrivateMethod";
    input.body.push_back(VisibilityKeyword{"private:"});
    input.body.push_back(method);
    method.name = "simplePrivateMethod2";
    input.body.push_back(method);

    // Act
    auto output = sut.generate(input);

    // Assert
    std::string regex = header + "class[ \t]*simpleClass" + ws + "\\{" + ws;
    regex += "public:" + ws + "void" + ws + "simplePublicMethod\\(\\);" + ws;
    regex += "protected:" + ws + "void" + ws + "simpleProtectedMethod\\(\\);" + ws;
    regex += "private:" + ws + "void" + ws + "simplePrivateMethod\\(\\);" + ws;
    regex += "void" + ws + "simplePrivateMethod2\\(\\);" + ws;
    regex += "\\};(.|\n)*";
    std::regex classRegex(regex);
    EXPECT_TRUE(std::regex_match(output, classRegex)) << output;
}

TEST(HeaderGenerator, AllKindsOfMembers)
{
    // Arrange
    auto config = std::make_shared<Config>();
    HeaderGenerator sut(config);

    Variable member;
    member.name = "simplePublicMember";
    member.type = Type{"int"};

    Class input;
    input.name = "simpleClass";
    input.body.push_back(VisibilityKeyword{"public:"});
    input.body.push_back(member);

    member.name = "simplePrivateMember";
    input.body.push_back(VisibilityKeyword{"private:"});
    input.body.push_back(member);
    member.name = "simplePrivateMember2";
    input.body.push_back(member);

    member.name = "simpleProtectedMember";
    input.body.push_back(VisibilityKeyword{"protected:"});
    input.body.push_back(member);

    // Act
    auto output = sut.generate(input);

    // Assert
    std::string regex = header + "class[ \t]*simpleClass" + ws + "\\{" + ws;
    regex += "public:" + ws + "int" + ws + "m_simplePublicMember;" + ws;
    regex += "private:" + ws + "int" + ws + "m_simplePrivateMember;" + ws;
    regex += "int" + ws + "m_simplePrivateMember2;" + ws;
    regex += "protected:" + ws + "int" + ws + "m_simpleProtectedMember;" + ws;
    regex += "\\};(.|\n)*";
    std::regex classRegex(regex);
    EXPECT_TRUE(std::regex_match(output, classRegex)) << output;
}

TEST(HeaderGenerator, MembersInStruct)
{
    // Arrange
    auto config = std::make_shared<Config>();
    HeaderGenerator sut(config);

    Variable member;
    member.name = "simplePublicMember";
    member.type = Type{"int"};

    Class input;
    input.name     = "simpleStruct";
    input.isStruct = true;
    input.body.push_back(VisibilityKeyword{"public:"});
    input.body.push_back(member);

    member.name = "simplePrivateMember";
    input.body.push_back(VisibilityKeyword{"private:"});
    input.body.push_back(member);
    member.name = "simplePrivateMember2";
    input.body.push_back(member);

    member.name = "simpleProtectedMember";
    input.body.push_back(VisibilityKeyword{"protected:"});
    input.body.push_back(member);

    // Act
    auto output = sut.generate(input);

    // Assert
    std::string regex = header + "struct[ \t]*simpleStruct" + ws + "\\{" + ws;
    regex += "public:" + ws + "int" + ws + "simplePublicMember;" + ws;
    regex += "private:" + ws + "int" + ws + "simplePrivateMember;" + ws;
    regex += "int" + ws + "simplePrivateMember2;" + ws;
    regex += "protected:" + ws + "int" + ws + "simpleProtectedMember;" + ws;
    regex += "\\};(.|\n)*";
    std::regex classRegex(regex);
    EXPECT_TRUE(std::regex_match(output, classRegex)) << output;
}

TEST(HeaderGenerator, ComplexTemplates)
{
    // Arrange
    auto config = std::make_shared<Config>();
    HeaderGenerator sut(config);

    Class input;
    input.name             = "simpleClass";
    input.externalIncludes = {"pair", "string", "vector"};
    input.localIncludes    = {"Visibility.h"};

    Variable member;
    member.name = "complexTemplate";
    member.type = Type{"std::vector", {Type{"std::pair", {Type{"Visibility"}, Type{"std::string"}}}}};
    input.body.push_back(member);

    // Act
    auto output = sut.generate(input);

    // Assert
    std::string regex = header + R"(\#include \<pair\>)" + ws + R"(\#include \<string\>)" + ws;
    regex += R"(\#include \<vector\>)" + ws + R"(\#include "Visibility.h")" + ws;
    regex += ws + "class[ \t]*simpleClass" + ws + "\\{" + ws;
    regex += "std::vector<std::pair<Visibility, std::string>>" + ws + "m_complexTemplate;" + ws;
    regex += "\\};(.|\n)*";
    std::regex classRegex(regex);
    EXPECT_TRUE(std::regex_match(output, classRegex)) << output;
}

} // namespace Cpp