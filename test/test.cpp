#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch2/catch.hpp"

#include "Parser.h"

static constexpr auto empty_puml = 
"@startuml \
@enduml";

static constexpr auto elements_puml = 
"@startuml\
abstract        abstract\
class           class\
@enduml";

/*static constexpr auto elements_puml = 
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
@enduml";*/

TEST_CASE( "empty diagram is parsed", "[Parser]" ) {
    Parser parser;
    PlantUML r = parser.parse(empty_puml);

    REQUIRE(r.elements.empty());
}

TEST_CASE( "diagram with all elements is parsed", "[Parser]" ) {
    Parser parser;
    PlantUML r = parser.parse(elements_puml);

    REQUIRE(r.elements.size() == 2);
}