#include "gtest/gtest.h"

#include "Parser.h"

#include "mocks/VisitorMock.h"

using ::testing::Eq;
using ::testing::Field;
using ::testing::Matcher;
using ::testing::Property;

Matcher<Expression> evalsTo(std::string_view str)
{
    return Property(&Expression::view, Eq(str));
}

Matcher<std::optional<Expression>> emptyOptional()
{
    return Property(&std::optional<Expression>::has_value, false);
}

Matcher<std::optional<Expression>> optionalEvalsTo(std::string_view str)
{
    return Property(&std::optional<Expression>::value, evalsTo(str));
}

void act(Parser& parser, VisitorMock& visitor, std::string_view puml)
{
    EXPECT_TRUE(parser.parse(puml));
    parser.visitAST(visitor);
}

TEST(ParserTest, EmptyDiagram)
{
    // Arrange
    VisitorMock visitor;
    Parser parser;

    static constexpr auto puml =
        R"(@startuml
@enduml)";

    // Assert Calls
    EXPECT_CALL(visitor, visitStart(emptyOptional()));

    // Act
    act(parser, visitor, puml);

    // Assert Results
}

TEST(ParserTest, NamedDiagram)
{
    // Arrange
    VisitorMock visitor;
    Parser parser;

    static constexpr auto puml =
        R"(@startuml empty
    @enduml)";

    // Assert Calls
    EXPECT_CALL(visitor, visitStart(optionalEvalsTo("empty")));

    // Act
    act(parser, visitor, puml);

    // Assert Results
}

TEST(ParserTest, SingleAbstract)
{
    // Arrange
    VisitorMock visitor;
    Parser parser;

    static constexpr auto puml =
        R"(@startuml
abstract        name
@enduml)";

    // Assert Calls
    EXPECT_CALL(visitor, visitClass(evalsTo("abstract"), evalsTo("name"), emptyOptional()));

    // Act
    act(parser, visitor, puml);

    // Assert Results
}