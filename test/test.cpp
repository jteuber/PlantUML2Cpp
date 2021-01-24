#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch2/catch.hpp"

#include "Parser.h"

static constexpr auto empty_puml = 
R"(@startuml
@enduml)";

TEST_CASE( "empty diagram is parsed", "[Parser]" ) {
    Parser parser;
    PlantUML r = parser.parse(empty_puml);

    REQUIRE(r.subData.empty());
}

static constexpr auto single_element_puml = 
R"(@startuml
abstract        abstract
@enduml)";

TEST_CASE( "diagram with one element is parsed", "[Parser]" ) {
    Parser parser;
    PlantUML r = parser.parse(single_element_puml);

    REQUIRE(r.subData.size() == 1);
}

static constexpr auto all_simple_containers_puml = 
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

TEST_CASE( "diagram with all simple containers is parsed", "[Parser]" ) {
    Parser parser;
    PlantUML r = parser.parse(all_simple_containers_puml);

    REQUIRE(r.subData.size() == 8);

    REQUIRE(r.subData[0].type == PlantUML::Type::Abstract);
    REQUIRE(r.subData[0].name == "abstract");
    REQUIRE(r.subData[1].type == PlantUML::Type::Annotation);
    REQUIRE(r.subData[1].name == "annotation");
    REQUIRE(r.subData[2].type == PlantUML::Type::Circle);
    REQUIRE(r.subData[2].name == "circle");
    REQUIRE(r.subData[3].type == PlantUML::Type::Class);
    REQUIRE(r.subData[3].name == "class");
    REQUIRE(r.subData[4].type == PlantUML::Type::Diamond);
    REQUIRE(r.subData[4].name == "diamond");
    REQUIRE(r.subData[5].type == PlantUML::Type::Entity);
    REQUIRE(r.subData[5].name == "entity");
    REQUIRE(r.subData[6].type == PlantUML::Type::Enum);
    REQUIRE(r.subData[6].name == "enum");
    REQUIRE(r.subData[7].type == PlantUML::Type::Interface);
    REQUIRE(r.subData[7].name == "interface");
}

static constexpr auto all_containers_puml = 
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

TEST_CASE( "diagram with all containers is parsed", "[Parser]" ) {
    Parser parser;
    PlantUML r = parser.parse(all_containers_puml);

    REQUIRE(r.subData.size() == 11);

    REQUIRE(r.subData[0].type == PlantUML::Type::Abstract);
    REQUIRE(r.subData[0].name == "abstract");
    REQUIRE(r.subData[1].type == PlantUML::Type::Abstract);
    REQUIRE(r.subData[1].name == "abstract class");
    REQUIRE(r.subData[2].type == PlantUML::Type::Annotation);
    REQUIRE(r.subData[2].name == "annotation");
    REQUIRE(r.subData[3].type == PlantUML::Type::Circle);
    REQUIRE(r.subData[3].name == "circle");
    REQUIRE(r.subData[4].type == PlantUML::Type::Circle);
    REQUIRE(r.subData[4].name == "circle_short_form");
    REQUIRE(r.subData[5].type == PlantUML::Type::Class);
    REQUIRE(r.subData[5].name == "class");
    REQUIRE(r.subData[6].type == PlantUML::Type::Diamond);
    REQUIRE(r.subData[6].name == "diamond");
    REQUIRE(r.subData[7].type == PlantUML::Type::Diamond);
    REQUIRE(r.subData[7].name == "diamond_short_form");
    REQUIRE(r.subData[8].type == PlantUML::Type::Entity);
    REQUIRE(r.subData[8].name == "entity");
    REQUIRE(r.subData[9].type == PlantUML::Type::Enum);
    REQUIRE(r.subData[9].name == "enum");
    REQUIRE(r.subData[10].type == PlantUML::Type::Interface);
    REQUIRE(r.subData[10].name == "interface");
}

static constexpr auto entity_with_empty_body_puml = 
R"(@startuml
entity test { }
@enduml)";

TEST_CASE( "entity with empty body is parsed", "[Parser]" ) {
    Parser parser;
    PlantUML r = parser.parse(entity_with_empty_body_puml);

    REQUIRE(r.subData.size() == 1);
    REQUIRE(r.subData[0].type == PlantUML::Type::Entity);
    REQUIRE(r.subData[0].name == "test");
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

    REQUIRE(r.subData.size() == 1);
    REQUIRE(r.subData[0].type == PlantUML::Type::Entity);
    REQUIRE(r.subData[0].name == "test");

    REQUIRE(r.subData[0].subData.size() == 1);
    REQUIRE(r.subData[0].subData[0].type == PlantUML::Type::Field);
    REQUIRE(r.subData[0].subData[0].name == "variable");
    REQUIRE(r.subData[0].subData[0].valueType == "var");
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

    REQUIRE(r.subData.size() == 1);
}

static constexpr auto class_with_two_methods_puml = 
R"(@startuml
class test {
    variable() : var
    variable2(input : bool) : var
}
@enduml)";

TEST_CASE( "class with two methods is parsed", "[Parser]" ) {
    Parser parser;
    PlantUML r = parser.parse(class_with_two_methods_puml);

    REQUIRE(r.subData.size() == 1);
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

    REQUIRE(r.subData.size() == 0);
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

    REQUIRE(r.subData.size() == 0);
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

    REQUIRE(r.subData.size() == 0);
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

    REQUIRE(r.subData.size() == 0);
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

    REQUIRE(r.subData.size() == 0);
}

static constexpr auto comments_puml = 
R"(@startuml
' simple comment
    ' indented comment
@enduml)";

TEST_CASE( "comments are parsed", "[Parser]" ) {
    Parser parser;
    PlantUML r = parser.parse(comments_puml);

    REQUIRE(r.subData.size() == 0);
}

static constexpr auto methods_external_puml = 
R"(@startuml
Object <|-- ArrayList

Object : equals()
ArrayList : Object[] elementData
ArrayList : size()
@enduml)";

TEST_CASE( "methods declared externally parsed", "[Parser]" ) {
    Parser parser;
    PlantUML r = parser.parse(methods_external_puml);

    REQUIRE(r.subData.size() == 1);
}

static constexpr auto packages_puml = 
R"(@startuml
package "Classic Collections" #DDDDDD {
  Object <|-- ArrayList
}

package net.sourceforge.plantuml {
  Object <|-- Demo1
  Demo1 *- Demo2
}
@enduml)";

TEST_CASE( "packages are parsed", "[Parser]" ) {
    Parser parser;
    PlantUML r = parser.parse(packages_puml);

    REQUIRE(r.subData.size() == 2);
    REQUIRE(r.subData[0].name == "Classic Collections");
    REQUIRE(r.subData[1].name == "net.sourceforge.plantuml");
}

static constexpr auto include_puml = 
R"(@startuml
!include something.iuml
@enduml)";

TEST_CASE( "include in diagram is ignored", "[Parser]" ) {
    Parser parser;
    PlantUML r = parser.parse(include_puml);

    REQUIRE(r.subData.size() == 0);
}

static constexpr auto enum_with_body_puml = 
R"(@startuml
enum TimeUnit {
  DAYS
  HOURS
  MINUTES
}

@enduml)";

TEST_CASE( "enum with body is parsed", "[Parser]" ) {
    Parser parser;
    PlantUML r = parser.parse(enum_with_body_puml);

    REQUIRE(r.subData.size() == 1);
}

static constexpr auto class_with_stereotype_and_spot_puml = 
R"(@startuml

class System << (S,#FF7700) Singleton >>
class Date << (D,orchid) >>

entity test << (T,white) Template >>
{
    test : var
}

@enduml)";

TEST_CASE( "class with stereotype and custom spot is parsed", "[Parser]" ) {
    Parser parser;
    PlantUML r = parser.parse(class_with_stereotype_and_spot_puml);

    REQUIRE(r.subData.size() == 3);
}

static constexpr auto hidden_relationships_puml = 
R"(@startuml

Together2 -[hidden]--> Bar1
Bar1 -[hidden]> Bar2

@enduml)";

TEST_CASE( "hidden relationships are parsed", "[Parser]" ) {
    Parser parser;
    PlantUML r = parser.parse(hidden_relationships_puml);

    REQUIRE(r.subData.size() == 0);
}

static constexpr auto real_live_example_part_1_puml = 
R"(@startuml

!include style/stylesheet.iuml

'~~~~~~~~~~~~~~~~~~~~
' data entities
'~~~~~~~~~~~~~~~~~~~~
package "data" 
{
    entity World

    entity Scene {
        name : std::string
    }

    enum SceneType {
        Static
        Scrolling
    }
    entity Prop {
        position : ScriptableValue<Point2D>
        size : ScriptableValue<Size2D>
    }
    entity Script {
        script : ChaiScript
    }
    entity Sprite
    entity Sound
    entity Texture
    entity AudioFile

    entity ScriptableValue << (T,white) Template >> 
    {
        value : T
        call : std::string
    }

    entity SceneFolder 
    {
        occurances : std::filesystem::path[]
    }
    entity FileStack 
    {
        base : std::filesystem::path
        patches : std::filesystem::path[]
        appends : std::filesystem::path[]
    }
    entity StackedFile 
    {
        contents : std::string
        loaded : bool
    }
}
@enduml)";

TEST_CASE( "real live example part 1 is parsed", "[Parser]" ) {
    Parser parser;
    PlantUML r = parser.parse(real_live_example_part_1_puml);

    REQUIRE(r.subData.size() == 1);
    REQUIRE(r.subData[0].name == "data");
    REQUIRE(r.subData[0].subData.size() == 13);
}

static constexpr auto real_live_example_part_2_puml = 
R"(@startuml
'~~~~~~~~~~~~~~~~~~~~
' algorithms
'~~~~~~~~~~~~~~~~~~~~

package "algorithms" {
    class WorldLoader
    {
        loadWorld() : World
    }

    class WorldFolderFinder
    {
        discoverWorldFolders() : std::vector<std::filesystem::path>
    }
    class SceneFinder
    {
        discoverSceneFolders(std::vector<std::filesystem::path> worldFolders) : std::vector<SceneFolder>
    }
    class SceneMapper
    {
        mapScene(SceneFolder scene) : SceneFolder
    }
    class SceneInitializer
    {
        loadScene(SceneFolder) : Scene
    }

    class SceneFinisher
    {
        finishLoadingScene(Scene)
    }

    class AudioLoader
    {
        loadAudioFile(StackedFile) : AudioFile
    }
    class TextureLoader
    {
        loadTexture(StackedFile) : Texture
    }
    class ScriptLoader
    {
        loadScript(StackedFile) : Script
    }

    class PropLoader
    {
        loadProp(StackedFile) : Prop
    }
    class FileStacker
    {
        assembleFile(FileStack) : StackedFile
    }
}

@enduml)";

TEST_CASE( "real live example part 2 is parsed", "[Parser]" ) {
    Parser parser;
    PlantUML r = parser.parse(real_live_example_part_2_puml);

    REQUIRE(r.subData.size() == 1);
    REQUIRE(r.subData[0].name == "algorithms");
    REQUIRE(r.subData[0].subData.size() == 11);
}

static constexpr auto real_live_example_part_3_puml = 
R"(@startuml
'~~~~~~~~~~~~~~~~~~~~
' relationships
'~~~~~~~~~~~~~~~~~~~~

' ====== data =======

' World
World *-- "0..*" Scene : "ScenesByName"

' Scene
Scene *-- "0..*" Prop
Scene *- "1" SceneType

' Prop
Prop "1" *-- "1" StackedFile
Prop "1" o-- "0..*" Script
Prop "1" o-- "0..1" Sprite
Prop "1" o-- "0..1" Sound

' Sound
Sound "1..*" *-- "1" AudioFile

' Sprite
Sprite "1..*" *-- "1" Texture

' AudioFile
StackedFile "1" --* "0..*" AudioFile

' Texture
StackedFile "1" --* "0..*" Texture

' Script
StackedFile "1" -* "0..*" Script

' SceneFolder
SceneFolder *-- "0..*" FileStack

' StackedFile
FileStack --o StackedFile

' ScriptableValue
ScriptableValue "1..*" *-- "1" Script

' ====== algorithms =======

WorldLoader *-- "1" WorldFolderFinder
WorldLoader *-- "1" SceneFinder
WorldLoader *-- "1" SceneMapper
WorldLoader *-- "1" SceneInitializer

SceneInitializer *-- "1" PropLoader
SceneInitializer *-- "1" FileStacker

SceneFinisher *--- "1" FileStacker
SceneFinisher *-- "1" AudioLoader
SceneFinisher *-- "1" TextureLoader
SceneFinisher *-- "1" ScriptLoader

WorldFolderFinder -> SceneFinder
SceneFinder -> SceneMapper
SceneMapper -> SceneInitializer

'~~~~~~~~~~~~~~~~~~~~
' layouting
'~~~~~~~~~~~~~~~~~~~~
WorldLoader -[hidden]> SceneFinisher

@enduml)";

TEST_CASE( "real live example part 3 is parsed", "[Parser]" ) {
    Parser parser;
    PlantUML r = parser.parse(real_live_example_part_3_puml);

    REQUIRE(r.subData.size() == 0);
}