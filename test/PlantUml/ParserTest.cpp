#include "gtest/gtest.h"

#include "PlantUml/ModelElement.h"
#include "PlantUml/Parser.h"

#include "mocks/VisitorMock.h"

using testing::Return;

namespace PlantUml {

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

    Container c{{}, "", ContainerType::Document};

    // Assert Calls
    EXPECT_CALL(visitor, visit(c));

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

    Container c{{"empty"}, "", ContainerType::Document};

    // Assert Calls
    EXPECT_CALL(visitor, visit(c));

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
        R"(
@startuml
abstract        name
@enduml)";

    Container c{{}, "", ContainerType::Document};
    Element e{{"name"}, "", ' ', {}, {}, ElementType::Abstract};
    End ee{EndType::Element};
    End ec{EndType::Document};

    // Assert Calls
    EXPECT_CALL(visitor, visit(c));
    EXPECT_CALL(visitor, visit(e));
    EXPECT_CALL(visitor, visit(ee));
    EXPECT_CALL(visitor, visit(ec));

    // Act
    act(parser, visitor, puml);

    // Assert Results
}

TEST(ParserTest, NoDescend)
{
    // Arrange
    VisitorMock visitor;
    Parser parser;

    static constexpr auto puml =
        R"(
@startuml
abstract        name
@enduml)";

    Container c{{}, "", ContainerType::Document};
    Element e{{"name"}, "", ' ', {}, {}, ElementType::Abstract};
    End ee{EndType::Element};
    End ec{EndType::Document};

    // Assert Calls
    EXPECT_CALL(visitor, visit(c));
    EXPECT_CALL(visitor, visit(e)).WillOnce(Return(false));
    EXPECT_CALL(visitor, visit(ee)).Times(0);
    EXPECT_CALL(visitor, visit(ec));

    // Act
    act(parser, visitor, puml);

    // Assert Results
}

TEST(ParserTest, AllElements)
{
    // Arrange
    VisitorMock visitor;
    Parser parser;

    static constexpr auto puml =
        R"(@startuml
abstract        abstract
abstract class  "abstract class"
annotation      annotation
circle          circle
()              circle_short_form
class           class
diamond         diamond
<>              diamond_short_form
entity          entity
enum            enum
interface       interface
@enduml)";

    Element a{{"abstract"}, "", ' ', {}, {}, ElementType::Abstract};
    Element ac{{"abstract class"}, "", ' ', {}, {}, ElementType::Abstract};
    Element an{{"annotation"}, "", ' ', {}, {}, ElementType::Annotation};
    Element c{{"class"}, "", ' ', {}, {}, ElementType::Class};
    Element ent{{"entity"}, "", ' ', {}, {}, ElementType::Entity};
    Element enu{{"enum"}, "", ' ', {}, {}, ElementType::Enum};
    Element i{{"interface"}, "", ' ', {}, {}, ElementType::Interface};

    // Assert Calls
    EXPECT_CALL(visitor, visit(a));
    EXPECT_CALL(visitor, visit(ac));
    EXPECT_CALL(visitor, visit(an));
    EXPECT_CALL(visitor, visit(c));
    EXPECT_CALL(visitor, visit(ent));
    EXPECT_CALL(visitor, visit(enu));
    EXPECT_CALL(visitor, visit(i));

    // Act
    act(parser, visitor, puml);

    // Assert Results
}

TEST(ParserTest, EmptyBody)
{
    // Arrange
    VisitorMock visitor;
    Parser parser;

    static constexpr auto puml =
        R"(@startuml
entity test { }
@enduml)";

    Container c{{}, "", ContainerType::Document};
    Element e{{"test"}, "", ' ', {}, {}, ElementType::Entity};
    End ee{EndType::Element};
    End ec{EndType::Document};

    // Assert Calls
    EXPECT_CALL(visitor, visit(c));
    EXPECT_CALL(visitor, visit(e));
    EXPECT_CALL(visitor, visit(ee));
    EXPECT_CALL(visitor, visit(ec));

    // Act
    act(parser, visitor, puml);

    // Assert Results
}

TEST(ParserTest, SingleVar)
{
    // Arrange
    VisitorMock visitor;
    Parser parser;

    static constexpr auto puml =
        R"(@startuml
entity test {
    variable : var
}
@enduml)";

    Element e{{"test"}, "", ' ', {}, {}, ElementType::Entity};
    Variable v{"variable", {"var"}, {}, Visibility::Unspecified, Modifier::None};
    End ee{EndType::Element};
    End ec{EndType::Document};

    // Assert Calls
    EXPECT_CALL(visitor, visit(e));
    EXPECT_CALL(visitor, visit(v));
    EXPECT_CALL(visitor, visit(ee));
    EXPECT_CALL(visitor, visit(ec));

    // Act
    act(parser, visitor, puml);

    // Assert Results
}

TEST(ParserTest, TwoMethods)
{
    // Arrange
    VisitorMock visitor;
    Parser parser;

    static constexpr auto puml =
        R"(@startuml
class test {
    method() : var
    method2(input : bool) : var
}
@enduml)";

    Element e{{"test"}, "", ' ', {}, {}, ElementType::Class};
    Method m1{"method", {"var"}, {}, Visibility::Unspecified, Modifier::None};
    Method m2{"method2", {"var"}, {}, Visibility::Unspecified, Modifier::None};
    Parameter p{"input", {"bool"}};

    // Assert Calls
    EXPECT_CALL(visitor, visit(e));
    EXPECT_CALL(visitor, visit(m1));
    EXPECT_CALL(visitor, visit(m2));
    EXPECT_CALL(visitor, visit(p));

    // Act
    act(parser, visitor, puml);

    // Assert Results
}

TEST(ParserTest, RelationshipsLeft)
{
    // Arrange
    VisitorMock visitor;
    Parser parser;

    static constexpr auto puml =
        R"(@startuml
Class01 <|-- Class02
Class03 *-- Class04
Class05 o-- Class06
Class07 <-- Class08
@enduml)";

    Relationship r1{{"Class02"}, {"Class01"}, "", "", "", false, RelationshipType::Extension};
    Relationship r2{{"Class03"}, {"Class04"}, "", "", "", false, RelationshipType::Composition};
    Relationship r3{{"Class05"}, {"Class06"}, "", "", "", false, RelationshipType::Aggregation};
    Relationship r4{{"Class08"}, {"Class07"}, "", "", "", false, RelationshipType::Usage};

    // Assert Calls
    EXPECT_CALL(visitor, visit(r1));
    EXPECT_CALL(visitor, visit(r2));
    EXPECT_CALL(visitor, visit(r3));
    EXPECT_CALL(visitor, visit(r4));

    // Act
    act(parser, visitor, puml);

    // Assert Results
}

TEST(ParserTest, RelationshipsRight)
{
    // Arrange
    VisitorMock visitor;
    Parser parser;

    static constexpr auto puml =
        R"(@startuml
Class01 --|> Class02
Class03 --* Class04
Class05 --o Class06
Class07 --> Class08
@enduml)";

    Relationship r1{{"Class01"}, {"Class02"}, "", "", "", false, RelationshipType::Extension};
    Relationship r2{{"Class04"}, {"Class03"}, "", "", "", false, RelationshipType::Composition};
    Relationship r3{{"Class06"}, {"Class05"}, "", "", "", false, RelationshipType::Aggregation};
    Relationship r4{{"Class07"}, {"Class08"}, "", "", "", false, RelationshipType::Usage};

    // Assert Calls
    EXPECT_CALL(visitor, visit(r1));
    EXPECT_CALL(visitor, visit(r2));
    EXPECT_CALL(visitor, visit(r3));
    EXPECT_CALL(visitor, visit(r4));

    // Act
    act(parser, visitor, puml);

    // Assert Results
}

TEST(ParserTest, RelationshipsWithLabels)
{
    // Arrange
    VisitorMock visitor;
    Parser parser;

    static constexpr auto puml =
        R"(@startuml
Class01 "1" *-- "many" Class02 : contains

Class03 o-- Class04 : aggregation

Class05 --|> "1" Class06
@enduml)";

    Relationship r1{{"Class01"}, {"Class02"}, "1", "many", "contains", false, RelationshipType::Composition};
    Relationship r2{{"Class03"}, {"Class04"}, "", "", "aggregation", false, RelationshipType::Aggregation};
    Relationship r3{{"Class05"}, {"Class06"}, "", "1", "", false, RelationshipType::Extension};

    // Assert Calls
    EXPECT_CALL(visitor, visit(r1));
    EXPECT_CALL(visitor, visit(r2));
    EXPECT_CALL(visitor, visit(r3));

    // Act
    act(parser, visitor, puml);

    // Assert Results
}

TEST(ParserTest, IncludesAndComments)
{
    // Arrange
    VisitorMock visitor;
    Parser parser;

    static constexpr auto puml =
        R"(@startuml
!include something.iuml
' simple comment
    ' indented comment
@enduml)";

    Container c{{}, "", ContainerType::Document};
    End ec{EndType::Document};

    // Assert Calls
    EXPECT_CALL(visitor, visit(An<const Variable&>())).Times(0);
    EXPECT_CALL(visitor, visit(An<const Method&>())).Times(0);
    EXPECT_CALL(visitor, visit(An<const Relationship&>())).Times(0);
    EXPECT_CALL(visitor, visit(An<const Container&>())).Times(0);
    EXPECT_CALL(visitor, visit(An<const Element&>())).Times(0);
    EXPECT_CALL(visitor, visit(An<const Note&>())).Times(0);
    EXPECT_CALL(visitor, visit(An<const Separator&>())).Times(0);
    EXPECT_CALL(visitor, visit(An<const Enumerator&>())).Times(0);
    EXPECT_CALL(visitor, visit(An<const Parameter&>())).Times(0);
    EXPECT_CALL(visitor, visit(An<const End&>())).Times(0);
    EXPECT_CALL(visitor, visit(c)).Times(1);
    EXPECT_CALL(visitor, visit(ec)).Times(1);

    // Act
    act(parser, visitor, puml);

    // Assert Results
}

TEST(ParserTest, ExternalVarsAndMethods)
{
    // Arrange
    VisitorMock visitor;
    Parser parser;

    static constexpr auto puml =
        R"(@startuml
Object : equals()
ArrayList : Object[] elementData
ArrayList : size() : int
@enduml)";

    Method m1{"equals", {}, {"Object"}, Visibility::Unspecified, Modifier::None};
    Variable v{"elementData", {"Object[]"}, {"ArrayList"}, Visibility::Unspecified, Modifier::None};
    Method m2{"size", {"int"}, {"ArrayList"}, Visibility::Unspecified, Modifier::None};

    // Assert Calls
    EXPECT_CALL(visitor, visit(m1));
    EXPECT_CALL(visitor, visit(v));
    EXPECT_CALL(visitor, visit(m2));

    // Act
    act(parser, visitor, puml);

    // Assert Results
}

TEST(ParserTest, Packages)
{
    // Arrange
    VisitorMock visitor;
    Parser parser;

    static constexpr auto puml =
        R"(@startuml
package "Classic Collections" #DDDDDD {
  Object <|-- ArrayList
}

package "net.sourceforge.plantuml" {
  Object <|-- Demo1
  Demo1 *- Demo2
}
@enduml)";

    Container c{{}, "", ContainerType::Document};
    Container p1{{"Classic Collections"}, "", ContainerType::Package};
    Container p2{{"net.sourceforge.plantuml"}, "", ContainerType::Package};
    End ep{EndType::Package};
    End ed{EndType::Document};

    // Assert Calls
    EXPECT_CALL(visitor, visit(c));
    EXPECT_CALL(visitor, visit(p1));
    EXPECT_CALL(visitor, visit(p2));
    EXPECT_CALL(visitor, visit(ep)).Times(2);
    EXPECT_CALL(visitor, visit(ed)).Times(1);

    // Act
    act(parser, visitor, puml);

    // Assert Results
}

TEST(ParserTest, Namespaces)
{
    // Arrange
    VisitorMock visitor;
    Parser parser;

    static constexpr auto puml =
        R"(@startuml
namespace net #DDDDDD {
}
namespace "net.foo" {
}

@enduml)";

    Container c{{}, "", ContainerType::Document};
    Container ns1{{"net"}, "", ContainerType::Namespace};
    Container ns2{{"net.foo"}, "", ContainerType::Namespace};
    End ens{EndType::Namespace};
    End ed{EndType::Document};

    // Assert Calls
    EXPECT_CALL(visitor, visit(c));
    EXPECT_CALL(visitor, visit(ns1));
    EXPECT_CALL(visitor, visit(ns2));
    EXPECT_CALL(visitor, visit(ens)).Times(2);
    EXPECT_CALL(visitor, visit(ed)).Times(1);

    // Act
    act(parser, visitor, puml);

    // Assert Results
}

TEST(ParserTest, NamespaceSeparator)
{
    // Arrange
    VisitorMock visitor;
    Parser parser;

    static constexpr auto puml =
        R"(@startuml

set namespaceSeparator ::
class X1::X2::foo {
  some info
}

@enduml)";

    Element e{{"X1", "X2", "foo"}, "", ' ', {}, {}, ElementType::Class};

    // Assert Calls
    EXPECT_CALL(visitor, visit(e));

    // Act
    act(parser, visitor, puml);

    // Assert Results
}

TEST(ParserTest, Enum)
{
    // Arrange
    VisitorMock visitor;
    Parser parser;

    static constexpr auto puml =
        R"(@startuml
enum TimeUnit {
  DAYS
  HOURS
  MINUTES
}

@enduml)";

    Element e{{"TimeUnit"}, "", ' ', {}, {}, ElementType::Enum};
    Enumerator en1{"DAYS"};
    Enumerator en2{"HOURS"};
    Enumerator en3{"MINUTES"};

    // Assert Calls
    EXPECT_CALL(visitor, visit(e));
    EXPECT_CALL(visitor, visit(en1));
    EXPECT_CALL(visitor, visit(en2));
    EXPECT_CALL(visitor, visit(en3));

    // Act
    act(parser, visitor, puml);

    // Assert Results
}

TEST(ParserTest, Stereotypes)
{
    // Arrange
    VisitorMock visitor;
    Parser parser;

    static constexpr auto puml =
        R"(@startuml

class System << (S,#FF7700) Singleton >>
class Date << (D,orchid) >>

entity test << (T,white) Template >>
{
    test : var
}

@enduml)";

    Element e1{{"System"}, "Singleton", 'S', {}, {}, ElementType::Class};
    Element e2{{"Date"}, "", 'D', {}, {}, ElementType::Class};
    Element e3{{"test"}, "Template", 'T', {}, {}, ElementType::Entity};

    // Assert Calls
    EXPECT_CALL(visitor, visit(e1));
    EXPECT_CALL(visitor, visit(e2));
    EXPECT_CALL(visitor, visit(e3));

    // Act
    act(parser, visitor, puml);

    // Assert Results
}

TEST(ParserTest, ImplementsExtends)
{
    // Arrange
    VisitorMock visitor;
    Parser parser;

    static constexpr auto puml =
        R"(@startuml

interface Iface

class Impl implements Iface
class Ext extends Impl

@enduml)";

    Element e1{{"Iface"}, "", ' ', {}, {}, ElementType::Interface};
    Element e2{{"Impl"}, "", ' ', {"Iface"}, {}, ElementType::Class};
    Element e3{{"Ext"}, "", ' ', {}, {"Impl"}, ElementType::Class};

    // Assert Calls
    EXPECT_CALL(visitor, visit(e1));
    EXPECT_CALL(visitor, visit(e2));
    EXPECT_CALL(visitor, visit(e3));

    // Act
    act(parser, visitor, puml);

    // Assert Results
}

} // namespace PlantUml