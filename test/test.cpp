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
abstract class  abstract_class
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