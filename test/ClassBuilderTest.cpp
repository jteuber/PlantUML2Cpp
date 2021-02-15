#include "gtest/gtest.h"

#include "ClassBuilder.h"
#include "Parser.h"


void act(Parser& parser, ClassBuilder& sut, std::string_view puml)
{
    EXPECT_TRUE(parser.parse(puml));
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

    // Assert Results
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

    // Assert Results
    EXPECT_TRUE(sut.results().size() == 1);
    EXPECT_TRUE(sut.results()[0].name == "name");
    EXPECT_TRUE(sut.results()[0].type == Class::Type::Abstract);
    EXPECT_TRUE(sut.results()[0].namespaceStack.empty());
}