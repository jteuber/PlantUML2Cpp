#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch2/catch.hpp"

#include "Parser.h"

static constexpr auto empty_puml = 
"@startuml \
@enduml";

TEST_CASE( "empty diagram is parsed", "[Parser]" ) {
    Parser parser;
    PlantUML r = parser.parse(empty_puml);

    REQUIRE(r.elements.empty());
}

static constexpr auto single_element_puml = 
"@startuml\
abstract        abstract\
@enduml";

TEST_CASE( "diagram with one element is parsed", "[Parser]" ) {
    Parser parser;
    PlantUML r = parser.parse(single_element_puml);

    REQUIRE(r.elements.size() == 1);
}

static constexpr auto all_simple_elements_puml = 
"@startuml\
abstract        abstract\
annotation      annotation\
circle          circle\
class           class\
diamond         diamond\
entity          entity\
enum            enum\
interface       interface\
@enduml";

TEST_CASE( "diagram with all simple elements is parsed", "[Parser]" ) {
    Parser parser;
    PlantUML r = parser.parse(all_simple_elements_puml);

    REQUIRE(r.elements.size() == 8);
}

static constexpr auto all_elements_puml = 
"@startuml\
abstract        abstract\
abstract class  abstract_class\
annotation      annotation\
circle          circle\
()              circle_short_form\
class           class\
diamond         diamond\
<>              diamond_short_form\
entity          entity\
enum            enum\
interface       interface\
@enduml";

TEST_CASE( "diagram with all elements is parsed", "[Parser]" ) {
    Parser parser;
    PlantUML r = parser.parse(all_elements_puml);

    REQUIRE(r.elements.size() == 11);
}

static constexpr auto entity_with_body_puml = 
"@startuml\
entity test {\
    variable : var\
}\
@enduml";

TEST_CASE( "entity with body is parsed", "[Parser]" ) {
    Parser parser;
    PlantUML r = parser.parse(entity_with_body_puml);

    REQUIRE(r.elements.size() == 1);
}