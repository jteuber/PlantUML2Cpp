#include "gtest/gtest.h"

#include <regex>

#include "Cpp/Class.h"
#include "Cpp/HeaderGenerator.h"

const std::string ws     = "( |\t|\n)*";
const std::string header = "#pragma once" + ws;

TEST(HeaderGenerator, EmptyClass)
{
    // Arrange
    auto config = std::make_shared<Cpp::Config>();
    Cpp::HeaderGenerator sut(config);

    Cpp::Class input;
    input.name = "empty";
    input.type = Cpp::Class::Type::Class;

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
    auto config = std::make_shared<Cpp::Config>();
    Cpp::HeaderGenerator sut(config);

    Cpp::Method method;
    method.name       = "simpleMethod";
    method.returnType = "void";
    method.visibility = Cpp::Visibility ::Public;

    Cpp::Class input;
    input.name = "simpleClass";
    input.type = Cpp::Class::Type::Class;
    input.methods.push_back(method);

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
    auto config = std::make_shared<Cpp::Config>();
    Cpp::HeaderGenerator sut(config);

    Cpp::Method method;
    method.name       = "simpleMethod";
    method.returnType = "void";
    method.visibility = Cpp::Visibility ::Private;

    Cpp::Class input;
    input.name = "simpleClass";
    input.type = Cpp::Class::Type::Class;
    input.methods.push_back(method);

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
    auto config = std::make_shared<Cpp::Config>();
    Cpp::HeaderGenerator sut(config);

    Cpp::Method method;
    method.name       = "simpleMethod";
    method.returnType = "void";
    method.visibility = Cpp::Visibility ::Protected;

    Cpp::Class input;
    input.name = "simpleClass";
    input.type = Cpp::Class::Type::Class;
    input.methods.push_back(method);

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
    auto config = std::make_shared<Cpp::Config>();
    Cpp::HeaderGenerator sut(config);

    Cpp::Method method;
    method.name       = "simplePublicMethod";
    method.returnType = "void";
    method.visibility = Cpp::Visibility ::Public;

    Cpp::Class input;
    input.name = "simpleClass";
    input.type = Cpp::Class::Type::Class;
    input.methods.push_back(method);

    method.name       = "simplePrivateMethod";
    method.visibility = Cpp::Visibility ::Private;
    input.methods.push_back(method);
    method.name = "simplePrivateMethod2";
    input.methods.push_back(method);

    method.name       = "simpleProtectedMethod";
    method.visibility = Cpp::Visibility ::Protected;
    input.methods.push_back(method);

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
    auto config = std::make_shared<Cpp::Config>();
    Cpp::HeaderGenerator sut(config);

    Cpp::Variable member;
    member.name       = "simplePublicMember";
    member.type       = "int";
    member.visibility = Cpp::Visibility ::Public;

    Cpp::Class input;
    input.name = "simpleClass";
    input.type = Cpp::Class::Type::Class;
    input.variables.push_back(member);

    member.name       = "simplePrivateMember";
    member.visibility = Cpp::Visibility ::Private;
    input.variables.push_back(member);
    member.name = "simplePrivateMember2";
    input.variables.push_back(member);

    member.name       = "simpleProtectedMember";
    member.visibility = Cpp::Visibility ::Protected;
    input.variables.push_back(member);

    // Act
    auto output = sut.generate(input);

    // Assert
    std::string regex = header + "class[ \t]*simpleClass" + ws + "\\{" + ws;
    regex += "public:" + ws + "int" + ws + "m_simplePublicMember;" + ws;
    regex += "protected:" + ws + "int" + ws + "m_simpleProtectedMember;" + ws;
    regex += "private:" + ws + "int" + ws + "m_simplePrivateMember;" + ws;
    regex += "int" + ws + "m_simplePrivateMember2;" + ws;
    regex += "\\};(.|\n)*";
    std::regex classRegex(regex);
    EXPECT_TRUE(std::regex_match(output, classRegex)) << output;
}

TEST(HeaderGenerator, MembersInStruct)
{
    // Arrange
    auto config = std::make_shared<Cpp::Config>();
    Cpp::HeaderGenerator sut(config);

    Cpp::Variable member;
    member.name       = "simplePublicMember";
    member.type       = "int";
    member.visibility = Cpp::Visibility ::Public;

    Cpp::Class input;
    input.name = "simpleStruct";
    input.type = Cpp::Class::Type::Struct;
    input.variables.push_back(member);

    member.name       = "simplePrivateMember";
    member.visibility = Cpp::Visibility ::Private;
    input.variables.push_back(member);
    member.name = "simplePrivateMember2";
    input.variables.push_back(member);

    member.name       = "simpleProtectedMember";
    member.visibility = Cpp::Visibility ::Protected;
    input.variables.push_back(member);

    // Act
    auto output = sut.generate(input);

    // Assert
    std::string regex = header + "struct[ \t]*simpleStruct" + ws + "\\{" + ws;
    regex += "public:" + ws + "int" + ws + "simplePublicMember;" + ws;
    regex += "protected:" + ws + "int" + ws + "simpleProtectedMember;" + ws;
    regex += "private:" + ws + "int" + ws + "simplePrivateMember;" + ws;
    regex += "int" + ws + "simplePrivateMember2;" + ws;
    regex += "\\};(.|\n)*";
    std::regex classRegex(regex);
    EXPECT_TRUE(std::regex_match(output, classRegex)) << output;
}

TEST(HeaderGenerator, MethodsAndMembers)
{
    // Arrange
    auto config = std::make_shared<Cpp::Config>();
    Cpp::HeaderGenerator sut(config);

    Cpp::Class input;
    input.name = "simpleClass";
    input.type = Cpp::Class::Type::Class;

    Cpp::Variable member;
    member.name       = "simplePublicMember";
    member.type       = "int";
    member.visibility = Cpp::Visibility ::Public;
    input.variables.push_back(member);

    member.name       = "simplePrivateMember";
    member.visibility = Cpp::Visibility ::Private;
    input.variables.push_back(member);

    member.name = "simplePrivateMember2";
    input.variables.push_back(member);

    member.name       = "simpleProtectedMember";
    member.visibility = Cpp::Visibility ::Protected;
    input.variables.push_back(member);

    Cpp::Method method;
    method.name       = "simplePublicMethod";
    method.returnType = "void";
    method.visibility = Cpp::Visibility ::Public;
    input.methods.push_back(method);

    method.name       = "simplePrivateMethod";
    method.visibility = Cpp::Visibility ::Private;
    input.methods.push_back(method);
    method.name = "simplePrivateMethod2";
    input.methods.push_back(method);

    method.name       = "simpleProtectedMethod";
    method.visibility = Cpp::Visibility ::Protected;
    input.methods.push_back(method);

    // Act
    auto output = sut.generate(input);

    // Assert
    std::string regex = header + "class[ \t]*simpleClass" + ws + "\\{" + ws;
    regex += "public:" + ws + "void" + ws + "simplePublicMethod\\(\\);" + ws;
    regex += "public:" + ws + "int" + ws + "m_simplePublicMember;" + ws;
    regex += "protected:" + ws + "void" + ws + "simpleProtectedMethod\\(\\);" + ws;
    regex += "protected:" + ws + "int" + ws + "m_simpleProtectedMember;" + ws;
    regex += "private:" + ws + "void" + ws + "simplePrivateMethod\\(\\);" + ws;
    regex += "void" + ws + "simplePrivateMethod2\\(\\);" + ws;
    regex += "private:" + ws + "int" + ws + "m_simplePrivateMember;" + ws;
    regex += "int" + ws + "m_simplePrivateMember2;" + ws;
    regex += "\\};(.|\n)*";
    std::regex classRegex(regex);
    EXPECT_TRUE(std::regex_match(output, classRegex)) << output;
}

TEST(HeaderGenerator, CompositionMember)
{
    // Arrange
    auto config = std::make_shared<Cpp::Config>();
    Cpp::HeaderGenerator sut(config);

    Cpp::Class input;
    input.name = "simpleClass";
    input.type = Cpp::Class::Type::Class;

    Cpp::Variable member;
    member.name       = "simpleComposition";
    member.type       = "ComplexType";
    member.visibility = Cpp::Visibility ::Public;
    member.source     = Cpp::Relationship::Composition;
    input.variables.push_back(member);

    member.name        = "manyComposition";
    member.type        = "OtherComplexType";
    member.visibility  = Cpp::Visibility ::Public;
    member.source      = Cpp::Relationship::Composition;
    member.cardinality = "0..*";
    input.variables.push_back(member);

    // Act
    auto output = sut.generate(input);

    // Assert
    std::string regex = header + R"(\#include \<vector\>)" + ws;
    regex += R"(\#include "ComplexType.h")" + ws + R"(\#include "OtherComplexType.h")" + ws;
    regex += ws + "class[ \t]*simpleClass" + ws + "\\{" + ws;
    regex += "public:" + ws;
    regex += "ComplexType" + ws + "m_simpleComposition;" + ws;
    regex += "std::vector<OtherComplexType>" + ws + "m_manyComposition;" + ws;
    regex += "\\};(.|\n)*";
    std::regex classRegex(regex);
    EXPECT_TRUE(std::regex_match(output, classRegex)) << output;
}

TEST(HeaderGenerator, ComplexTemplates)
{
    // Arrange
    auto config = std::make_shared<Cpp::Config>();
    Cpp::HeaderGenerator sut(config);

    Cpp::Class input;
    input.name = "simpleClass";
    input.type = Cpp::Class::Type::Class;

    Cpp::Variable member;
    member.name       = "complexTemplate";
    member.type       = "std::vector<std::pair<Visibility , std::string>>";
    member.visibility = Cpp::Visibility ::Public;
    input.variables.push_back(member);

    // Act
    auto output = sut.generate(input);

    // Assert
    std::string regex = header + R"(\#include \<pair\>)" + ws + R"(\#include \<string\>)" + ws;
    regex += R"(\#include \<vector\>)" + ws + R"(\#include "Visibility.h")" + ws;
    regex += ws + "class[ \t]*simpleClass" + ws + "\\{" + ws;
    regex += "public:" + ws;
    regex += "std::vector<std::pair<Visibility , std::string>>" + ws + "m_complexTemplate;" + ws;
    regex += "\\};(.|\n)*";
    std::regex classRegex(regex);
    EXPECT_TRUE(std::regex_match(output, classRegex)) << output;
}

TEST(HeaderGenerator, TemplateWithBuiltinType)
{
    // Arrange
    auto config = std::make_shared<Cpp::Config>();
    Cpp::HeaderGenerator sut(config);

    Cpp::Class input;
    input.name = "simpleClass";
    input.type = Cpp::Class::Type::Class;

    Cpp::Variable member;
    member.name       = "template";
    member.type       = "vector<int>";
    member.visibility = Cpp::Visibility ::Public;
    input.variables.push_back(member);

    // Act
    auto output = sut.generate(input);

    // Assert
    std::string regex = header + R"(\#include \<vector\>)" + ws;
    regex += "class[ \t]*simpleClass" + ws + "\\{" + ws;
    regex += "public:" + ws;
    regex += "std::vector<int>" + ws + "m_template;" + ws;
    regex += "\\};(.|\n)*";
    std::regex classRegex(regex);
    EXPECT_TRUE(std::regex_match(output, classRegex)) << output;
}

TEST(HeaderGenerator, UmlTypeTemplates)
{
    // Arrange
    auto config = std::make_shared<Cpp::Config>();
    Cpp::HeaderGenerator sut(config);

    Cpp::Class input;
    input.name = "simpleClass";
    input.type = Cpp::Class::Type::Class;

    Cpp::Variable member;
    member.name       = "complexTemplate";
    member.type       = "vector<string>";
    member.visibility = Cpp::Visibility ::Public;
    input.variables.push_back(member);

    // Act
    auto output = sut.generate(input);

    // Assert
    std::string regex = header + R"(\#include \<string\>)" + ws;
    regex += R"(\#include \<vector\>)" + ws;
    regex += "class[ \t]*simpleClass" + ws + "\\{" + ws;
    regex += "public:" + ws;
    regex += "std::vector<std::string>" + ws + "m_complexTemplate;" + ws;
    regex += "\\};(.|\n)*";
    std::regex classRegex(regex);
    EXPECT_TRUE(std::regex_match(output, classRegex)) << output;
}