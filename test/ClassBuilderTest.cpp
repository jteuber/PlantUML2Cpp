#include "gtest/gtest.h"

#include "ClassBuilder.h"
#include "Parser.h"


void act(Parser& parser, ClassBuilder& sut, std::string_view puml)
{
    ASSERT_TRUE(parser.parse(puml));
    parser.visitAST(sut);
}

TEST(ClassBuilderTest, EmptyDiagram)
{
    // Arrange
    ClassBuilder sut;
    Parser parser;

    static constexpr auto puml =
    R"(@startuml
@enduml)";

    // Act
    act(parser, sut, puml);

    // Assert
    EXPECT_TRUE(sut.results().empty());
}

TEST(ClassBuilderTest, SingleAbstract)
{
    // Arrange
    ClassBuilder sut;
    Parser parser;

    static constexpr auto puml =
    R"(@startuml
abstract        name
@enduml)";

    // Act
    act(parser, sut, puml);

    // Assert
    ASSERT_EQ(sut.results().size(), 1);

    EXPECT_EQ(sut.results()[0].name, "name");
    EXPECT_EQ(sut.results()[0].type, Class::Type::Abstract);
    EXPECT_TRUE(sut.results()[0].namespaceStack.empty());
}

TEST(ClassBuilderTest, AllSimpleContainers)
{
    // Arrange
    ClassBuilder sut;
    Parser parser;

    static constexpr auto puml =
        R"(@startuml
    abstract        abstract
    annotation      annotation
    circle          circle
    class           class
    diamond         diamond
    entity          entity
    enum            enum
    interface       interface
    @enduml)";

    // Act
    act(parser, sut, puml);

    // Assert
    ASSERT_EQ(sut.results().size(), 4);

    EXPECT_EQ(sut.results()[0].name, "abstract");
    EXPECT_EQ(sut.results()[0].type, Class::Type::Abstract);
    EXPECT_EQ(sut.results()[1].name, "class");
    EXPECT_EQ(sut.results()[1].type, Class::Type::Class);
    EXPECT_EQ(sut.results()[2].name, "entity");
    EXPECT_EQ(sut.results()[2].type, Class::Type::Struct);
    EXPECT_EQ(sut.results()[3].name, "interface");
    EXPECT_EQ(sut.results()[3].type, Class::Type::Interface);
}

TEST(ClassBuilderTest, EntityWithEmptyBody)
{
    // Arrange
    ClassBuilder sut;
    Parser parser;

    static constexpr auto puml =
        R"(@startuml
    entity test { }
    @enduml)";

    // Act
    act(parser, sut, puml);

    // Assert
    ASSERT_EQ(sut.results().size(), 1);

    EXPECT_EQ(sut.results()[0].name, "test");
    EXPECT_EQ(sut.results()[0].type, Class::Type::Struct);
    EXPECT_TRUE(sut.results()[0].variables.empty());
    EXPECT_TRUE(sut.results()[0].methods.empty());
}

TEST(ClassBuilderTest, EntityWithSingleVariable)
{
    // Arrange
    ClassBuilder sut;
    Parser parser;

    static constexpr auto puml =
    R"(@startuml
    entity test {
        variable : var
    }
    @enduml)";

    // Act
    act(parser, sut, puml);

    // Assert
    ASSERT_EQ(sut.results().size(), 1);
    
    EXPECT_EQ(sut.results()[0].name, "test");
    EXPECT_EQ(sut.results()[0].type, Class::Type::Struct);
    EXPECT_TRUE(sut.results()[0].methods.empty());

    ASSERT_EQ(sut.results()[0].variables.size(), 1);
    EXPECT_EQ(sut.results()[0].variables[0].name, "variable");
    EXPECT_EQ(sut.results()[0].variables[0].type, "var");
}

TEST(ClassBuilderTest, ClassWithTwoMethods)
{
    // Arrange
    ClassBuilder sut;
    Parser parser;

    static constexpr auto puml =
        R"(@startuml
    class test {
        method() : var
        method2(input : bool) : var
    }
    @enduml)";

    // Act
    act(parser, sut, puml);

    // Assert
    ASSERT_EQ(sut.results().size(), 1);
    
    EXPECT_EQ(sut.results()[0].name, "test");
    EXPECT_EQ(sut.results()[0].type, Class::Type::Class);
    EXPECT_TRUE(sut.results()[0].variables.empty());

    ASSERT_EQ(sut.results()[0].methods.size(), 2);
    EXPECT_EQ(sut.results()[0].methods[0].name, "method");
    EXPECT_EQ(sut.results()[0].methods[0].returnType, "var");
    EXPECT_TRUE(sut.results()[0].methods[0].parameters.empty());

    EXPECT_EQ(sut.results()[0].methods[1].name, "method2");
    EXPECT_EQ(sut.results()[0].methods[1].returnType, "var");

    ASSERT_EQ(sut.results()[0].methods[1].parameters.size(), 1);
    EXPECT_EQ(sut.results()[0].methods[1].parameters[0].name, "input");
    EXPECT_EQ(sut.results()[0].methods[1].parameters[0].type, "bool");
}
