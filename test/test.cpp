#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch2/catch.hpp"

#include "Parser.h"

static constexpr auto empty_puml = 
R"(@startuml
@enduml)";

TEST_CASE( "empty diagram is parsed", "[Parser]" ) {
    Parser parser;
    PlantUML r = parser.parse(empty_puml);

    REQUIRE(r.elements.empty());
}

static constexpr auto single_element_puml = 
R"(@startuml
abstract        abstract
@enduml)";

TEST_CASE( "diagram with one element is parsed", "[Parser]" ) {
    Parser parser;
    PlantUML r = parser.parse(single_element_puml);

    REQUIRE(r.elements.size() == 1);
}

static constexpr auto all_simple_elements_puml = 
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

TEST_CASE( "diagram with all simple elements is parsed", "[Parser]" ) {
    Parser parser;
    PlantUML r = parser.parse(all_simple_elements_puml);

    REQUIRE(r.elements.size() == 8);
    for (int i=0; i<8; ++i)
    {
        REQUIRE(r.elements[i].type == static_cast<PlantUML::Element::Type>(i));
    }
}

static constexpr auto all_elements_puml = 
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

TEST_CASE( "diagram with all elements is parsed", "[Parser]" ) {
    Parser parser;
    PlantUML r = parser.parse(all_elements_puml);

    REQUIRE(r.elements.size() == 11);
}

static constexpr auto entity_with_empty_body_puml = 
R"(@startuml
entity test { }
@enduml)";

TEST_CASE( "entity with empty body is parsed", "[Parser]" ) {
    Parser parser;
    PlantUML r = parser.parse(entity_with_empty_body_puml);

    REQUIRE(r.elements.size() == 1);
}

static constexpr auto entity_with_single_var_puml = 
R"(@startuml
entity test {
    variable : var
}
@enduml)";

TEST_CASE( "entity with single variable is parsed", "[Parser]" ) {
    Parser parser;
    PlantUML r = parser.parse(entity_with_single_var_puml);

    REQUIRE(r.elements.size() == 1);
}

static constexpr auto entity_with_two_var_puml = 
R"(@startuml
entity test {
    variable : var
    variable2 : var
}
@enduml)";

TEST_CASE( "entity with two variables is parsed", "[Parser]" ) {
    Parser parser;
    PlantUML r = parser.parse(entity_with_single_var_puml);

    REQUIRE(r.elements.size() == 1);
}

static constexpr auto relationships_left_puml = 
R"(@startuml
Class01 <|-- Class02
Class03 *-- Class04
Class05 o-- Class06
@enduml)";

TEST_CASE( "relationships left are parsed", "[Parser]" ) {
    Parser parser;
    PlantUML r = parser.parse(relationships_left_puml);

    REQUIRE(r.elements.size() == 0);
}

static constexpr auto relationships_right_puml = 
R"(@startuml
Class01 --|> Class02
Class03 --* Class04
Class05 --o Class06
@enduml)";

TEST_CASE( "relationships right are parsed", "[Parser]" ) {
    Parser parser;
    PlantUML r = parser.parse(relationships_right_puml);

    REQUIRE(r.elements.size() == 0);
}

static constexpr auto relationships_up_puml = 
R"(@startuml
Class01 <|- Class02
Class03 *- Class04
Class05 o- Class06
@enduml)";

TEST_CASE( "relationships up are parsed", "[Parser]" ) {
    Parser parser;
    PlantUML r = parser.parse(relationships_up_puml);

    REQUIRE(r.elements.size() == 0);
}

static constexpr auto relationships_down_puml = 
R"(@startuml
Class01 -|> Class02
Class03 -* Class04
Class05 -o Class06
@enduml)";

TEST_CASE( "relationships down are parsed", "[Parser]" ) {
    Parser parser;
    PlantUML r = parser.parse(relationships_down_puml);

    REQUIRE(r.elements.size() == 0);
}

static constexpr auto labels_on_relations_puml = 
R"(@startuml
Class01 "1" *-- "many" Class02 : contains

Class03 o-- Class04 : aggregation

Class05 --|> "1" Class06
@enduml)";

TEST_CASE( "labels on relationships are parsed", "[Parser]" ) {
    Parser parser;
    PlantUML r = parser.parse(labels_on_relations_puml);

    REQUIRE(r.elements.size() == 0);
}