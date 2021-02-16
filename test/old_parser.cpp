#include <gtest/gtest.h>

#include "Parser.h"

static constexpr auto empty_puml =
    R"(@startuml
@enduml)";

TEST_CASE("empty diagram is parsed", "[Parser]")
{
    Parser parser;
    PlantUMLPtr r = parser.parse(empty_puml);

    REQUIRE(r->subData.empty());
}

static constexpr auto named_puml =
    R"(@startuml empty
@enduml)";

TEST_CASE("named diagram is parsed", "[Parser]")
{
    Parser parser;
    PlantUMLPtr r = parser.parse(named_puml);

    REQUIRE(r->name == "empty");
}

static constexpr auto single_element_puml =
    R"(@startuml
abstract        abstract
@enduml)";

TEST_CASE("diagram with one element is parsed", "[Parser]")
{
    Parser parser;
    PlantUMLPtr r = parser.parse(single_element_puml);

    REQUIRE(r->subData.size() == 1);
    REQUIRE(r->subData[0]->type == PlantUML::Type::Abstract);
    REQUIRE(r->subData[0]->name == "abstract");
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

TEST_CASE("diagram with all simple containers is parsed", "[Parser]")
{
    Parser parser;
    PlantUMLPtr r = parser.parse(all_simple_containers_puml);

    REQUIRE(r->subData.size() == 8);

    REQUIRE(r->subData[0]->type == PlantUML::Type::Abstract);
    REQUIRE(r->subData[0]->name == "abstract");
    REQUIRE(r->subData[1]->type == PlantUML::Type::Annotation);
    REQUIRE(r->subData[1]->name == "annotation");
    REQUIRE(r->subData[2]->type == PlantUML::Type::Circle);
    REQUIRE(r->subData[2]->name == "circle");
    REQUIRE(r->subData[3]->type == PlantUML::Type::Class);
    REQUIRE(r->subData[3]->name == "class");
    REQUIRE(r->subData[4]->type == PlantUML::Type::Diamond);
    REQUIRE(r->subData[4]->name == "diamond");
    REQUIRE(r->subData[5]->type == PlantUML::Type::Entity);
    REQUIRE(r->subData[5]->name == "entity");
    REQUIRE(r->subData[6]->type == PlantUML::Type::Enum);
    REQUIRE(r->subData[6]->name == "enum");
    REQUIRE(r->subData[7]->type == PlantUML::Type::Interface);
    REQUIRE(r->subData[7]->name == "interface");
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

TEST_CASE("diagram with all containers is parsed", "[Parser]")
{
    Parser parser;
    PlantUMLPtr r = parser.parse(all_containers_puml);

    REQUIRE(r->subData.size() == 11);

    REQUIRE(r->subData[0]->type == PlantUML::Type::Abstract);
    REQUIRE(r->subData[0]->name == "abstract");
    REQUIRE(r->subData[1]->type == PlantUML::Type::Abstract);
    REQUIRE(r->subData[1]->name == "abstract class");
    REQUIRE(r->subData[2]->type == PlantUML::Type::Annotation);
    REQUIRE(r->subData[2]->name == "annotation");
    REQUIRE(r->subData[3]->type == PlantUML::Type::Circle);
    REQUIRE(r->subData[3]->name == "circle");
    REQUIRE(r->subData[4]->type == PlantUML::Type::Circle);
    REQUIRE(r->subData[4]->name == "circle_short_form");
    REQUIRE(r->subData[5]->type == PlantUML::Type::Class);
    REQUIRE(r->subData[5]->name == "class");
    REQUIRE(r->subData[6]->type == PlantUML::Type::Diamond);
    REQUIRE(r->subData[6]->name == "diamond");
    REQUIRE(r->subData[7]->type == PlantUML::Type::Diamond);
    REQUIRE(r->subData[7]->name == "diamond_short_form");
    REQUIRE(r->subData[8]->type == PlantUML::Type::Entity);
    REQUIRE(r->subData[8]->name == "entity");
    REQUIRE(r->subData[9]->type == PlantUML::Type::Enum);
    REQUIRE(r->subData[9]->name == "enum");
    REQUIRE(r->subData[10]->type == PlantUML::Type::Interface);
    REQUIRE(r->subData[10]->name == "interface");
}

static constexpr auto entity_with_empty_body_puml =
    R"(@startuml
entity test { }
@enduml)";

TEST_CASE("entity with empty body is parsed", "[Parser]")
{
    Parser parser;
    PlantUMLPtr r = parser.parse(entity_with_empty_body_puml);

    REQUIRE(r->subData.size() == 1);
    REQUIRE(r->subData[0]->type == PlantUML::Type::Entity);
    REQUIRE(r->subData[0]->name == "test");
}

static constexpr auto entity_with_single_var_puml =
    R"(@startuml
entity test {
    variable : var
}
@enduml)";

TEST_CASE("entity with single variable is parsed", "[Parser]")
{
    Parser parser;
    PlantUMLPtr r = parser.parse(entity_with_single_var_puml);

    REQUIRE(r->subData.size() == 1);
    REQUIRE(r->subData[0]->type == PlantUML::Type::Entity);
    REQUIRE(r->subData[0]->name == "test");

    REQUIRE(r->subData[0]->subData.size() == 1);
    REQUIRE(r->subData[0]->subData[0]->type == PlantUML::Type::Field);
    REQUIRE(r->subData[0]->subData[0]->name == "variable");
    REQUIRE(r->subData[0]->subData[0]->valueType == "var");
}

static constexpr auto entity_with_two_var_puml =
    R"(@startuml
entity test {
    variable : var
    variable2 : var
}
@enduml)";

TEST_CASE("entity with two variables is parsed", "[Parser]")
{
    Parser parser;
    PlantUMLPtr r = parser.parse(entity_with_two_var_puml);

    REQUIRE(r->subData.size() == 1);
    REQUIRE(r->subData[0]->type == PlantUML::Type::Entity);
    REQUIRE(r->subData[0]->name == "test");

    REQUIRE(r->subData[0]->subData.size() == 2);
    REQUIRE(r->subData[0]->subData[0]->type == PlantUML::Type::Field);
    REQUIRE(r->subData[0]->subData[0]->name == "variable");
    REQUIRE(r->subData[0]->subData[0]->valueType == "var");
    REQUIRE(r->subData[0]->subData[1]->type == PlantUML::Type::Field);
    REQUIRE(r->subData[0]->subData[1]->name == "variable2");
    REQUIRE(r->subData[0]->subData[1]->valueType == "var");
}

static constexpr auto class_with_two_methods_puml =
    R"(@startuml
class test {
    method() : var
    method2(input : bool) : var
}
@enduml)";

TEST_CASE("class with two methods is parsed", "[Parser]")
{
    Parser parser;
    PlantUMLPtr r = parser.parse(class_with_two_methods_puml);

    REQUIRE(r->subData.size() == 1);
    REQUIRE(r->subData[0]->type == PlantUML::Type::Class);
    REQUIRE(r->subData[0]->name == "test");

    REQUIRE(r->subData[0]->subData.size() == 2);
    REQUIRE(r->subData[0]->subData[0]->type == PlantUML::Type::Method);
    REQUIRE(r->subData[0]->subData[0]->name == "method");
    REQUIRE(r->subData[0]->subData[0]->valueType == "var");
    REQUIRE(r->subData[0]->subData[0]->subData.empty());
    REQUIRE(r->subData[0]->subData[1]->type == PlantUML::Type::Method);
    REQUIRE(r->subData[0]->subData[1]->name == "method2");
    REQUIRE(r->subData[0]->subData[1]->valueType == "var");
    REQUIRE(r->subData[0]->subData[1]->subData.size() == 1);
    REQUIRE(r->subData[0]->subData[1]->subData[0]->name == "input");
    REQUIRE(r->subData[0]->subData[1]->subData[0]->valueType == "bool");
}

static constexpr auto relationships_left_puml =
    R"(@startuml
Class01 <|-- Class02
Class03 *-- Class04
Class05 o-- Class06
@enduml)";

TEST_CASE("relationships left are parsed", "[Parser]")
{
    Parser parser;
    PlantUMLPtr r = parser.parse(relationships_left_puml);

    REQUIRE(r->subData.size() == 6);
    for (auto c : r->subData)
        REQUIRE(c->type == PlantUML::Type::Class);

    // Class02 extends Class01
    REQUIRE(r->subData[0]->name == "Class02");
    REQUIRE(r->subData[1]->name == "Class01");
    REQUIRE(r->subData[0]->valueType == "Class01");

    // Class03 has a data member of type Class04 as composition
    REQUIRE(r->subData[2]->name == "Class03");
    REQUIRE(r->subData[3]->name == "Class04");
    REQUIRE(r->subData[2]->subData.size() == 1);
    REQUIRE(r->subData[2]->subData[0]->type == PlantUML::Type::Composition);
    REQUIRE(r->subData[2]->subData[0]->valueType == "Class04");
    // no requirement on the name

    // Class05 has a data member of type Class06 as aggregation
    REQUIRE(r->subData[4]->name == "Class05");
    REQUIRE(r->subData[5]->name == "Class06");
    REQUIRE(r->subData[4]->subData.size() == 1);
    REQUIRE(r->subData[4]->subData[0]->type == PlantUML::Type::Aggregation);
    REQUIRE(r->subData[4]->subData[0]->valueType == "Class06");
}

static constexpr auto relationships_right_puml =
    R"(@startuml
Class01 --|> Class02
Class03 --* Class04
Class05 --o Class06
@enduml)";

TEST_CASE("relationships right are parsed", "[Parser]")
{
    Parser parser;
    PlantUMLPtr r = parser.parse(relationships_right_puml);

    REQUIRE(r->subData.size() == 6);
    for (auto c : r->subData)
        REQUIRE(c->type == PlantUML::Type::Class);

    // Class01 extends Class02
    REQUIRE(r->subData[0]->name == "Class01");
    REQUIRE(r->subData[1]->name == "Class02");
    REQUIRE(r->subData[0]->valueType == "Class02");

    // Class04 has a data member of type Class03 as composition
    REQUIRE(r->subData[3]->name == "Class03");
    REQUIRE(r->subData[2]->name == "Class04");
    REQUIRE(r->subData[2]->subData.size() == 1);
    REQUIRE(r->subData[2]->subData[0]->type == PlantUML::Type::Composition);
    REQUIRE(r->subData[2]->subData[0]->valueType == "Class03");
    // no requirement on the name

    // Class06 has a data member of type Class05 as aggregation
    REQUIRE(r->subData[5]->name == "Class05");
    REQUIRE(r->subData[4]->name == "Class06");
    REQUIRE(r->subData[4]->subData.size() == 1);
    REQUIRE(r->subData[4]->subData[0]->type == PlantUML::Type::Aggregation);
    REQUIRE(r->subData[4]->subData[0]->valueType == "Class05");
}

static constexpr auto relationships_up_puml =
    R"(@startuml
Class01 <|- Class02
Class03 *- Class04
Class05 o- Class06
@enduml)";

TEST_CASE("relationships up are parsed", "[Parser]")
{
    Parser parser;
    PlantUMLPtr r = parser.parse(relationships_up_puml);

    REQUIRE(r->subData.size() == 6);
    for (auto c : r->subData)
        REQUIRE(c->type == PlantUML::Type::Class);

    // Class02 extends Class01
    REQUIRE(r->subData[0]->name == "Class02");
    REQUIRE(r->subData[1]->name == "Class01");
    REQUIRE(r->subData[0]->valueType == "Class01");

    // Class03 has a data member of type Class04 as composition
    REQUIRE(r->subData[2]->name == "Class03");
    REQUIRE(r->subData[3]->name == "Class04");
    REQUIRE(r->subData[2]->subData.size() == 1);
    REQUIRE(r->subData[2]->subData[0]->type == PlantUML::Type::Composition);
    REQUIRE(r->subData[2]->subData[0]->valueType == "Class04");
    // no requirement on the name

    // Class05 has a data member of type Class06 as aggregation
    REQUIRE(r->subData[4]->name == "Class05");
    REQUIRE(r->subData[5]->name == "Class06");
    REQUIRE(r->subData[4]->subData.size() == 1);
    REQUIRE(r->subData[4]->subData[0]->type == PlantUML::Type::Aggregation);
    REQUIRE(r->subData[4]->subData[0]->valueType == "Class06");
}

static constexpr auto relationships_down_puml =
    R"(@startuml
Class01 -|> Class02
Class03 -* Class04
Class05 -o Class06
@enduml)";

TEST_CASE("relationships down are parsed", "[Parser]")
{
    Parser parser;
    PlantUMLPtr r = parser.parse(relationships_down_puml);

    REQUIRE(r->subData.size() == 6);
    for (auto c : r->subData)
        REQUIRE(c->type == PlantUML::Type::Class);

    // Class01 extends Class02
    REQUIRE(r->subData[0]->name == "Class01");
    REQUIRE(r->subData[1]->name == "Class02");
    REQUIRE(r->subData[0]->valueType == "Class02");

    // Class04 has a data member of type Class03 as composition
    REQUIRE(r->subData[3]->name == "Class03");
    REQUIRE(r->subData[2]->name == "Class04");
    REQUIRE(r->subData[2]->subData.size() == 1);
    REQUIRE(r->subData[2]->subData[0]->type == PlantUML::Type::Composition);
    REQUIRE(r->subData[2]->subData[0]->valueType == "Class03");
    // no requirement on the name

    // Class06 has a data member of type Class05 as aggregation
    REQUIRE(r->subData[5]->name == "Class05");
    REQUIRE(r->subData[4]->name == "Class06");
    REQUIRE(r->subData[4]->subData.size() == 1);
    REQUIRE(r->subData[4]->subData[0]->type == PlantUML::Type::Aggregation);
    REQUIRE(r->subData[4]->subData[0]->valueType == "Class05");
}

static constexpr auto labels_on_relations_puml =
    R"(@startuml
Class01 "1" *-- "many" Class02 : contains

Class03 o-- Class04 : aggregation

Class05 --|> "1" Class06
@enduml)";

TEST_CASE("labels on relationships are parsed", "[Parser]")
{
    Parser parser;
    PlantUMLPtr r = parser.parse(labels_on_relations_puml);

    REQUIRE(r->subData.size() == 6);
    for (auto c : r->subData)
        REQUIRE(c->type == PlantUML::Type::Class);

    // Class01 has a data member of type Class02 as composition
    REQUIRE(r->subData[0]->name == "Class01");
    REQUIRE(r->subData[1]->name == "Class02");
    REQUIRE(r->subData[0]->subData.size() == 1);
    REQUIRE(r->subData[0]->subData[0]->type == PlantUML::Type::Composition);
    REQUIRE(r->subData[0]->subData[0]->valueType == "Class02");
    REQUIRE(r->subData[0]->subData[0]->modifier == "many");
    REQUIRE(r->subData[0]->subData[0]->name == "contains");

    // Class03 has a data member of type Class04 as aggregation
    REQUIRE(r->subData[2]->name == "Class03");
    REQUIRE(r->subData[3]->name == "Class04");
    REQUIRE(r->subData[2]->subData.size() == 1);
    REQUIRE(r->subData[2]->subData[0]->type == PlantUML::Type::Aggregation);
    REQUIRE(r->subData[2]->subData[0]->valueType == "Class04");
    REQUIRE(r->subData[2]->subData[0]->name == "aggregation");

    // Class05 extends Class06
    REQUIRE(r->subData[5]->name == "Class06");
    REQUIRE(r->subData[4]->name == "Class05");
    REQUIRE(r->subData[4]->valueType == "Class06");
}

static constexpr auto comments_puml =
    R"(@startuml
' simple comment
    ' indented comment
@enduml)";

TEST_CASE("comments are ignored", "[Parser]")
{
    Parser parser;
    PlantUMLPtr r = parser.parse(comments_puml);

    REQUIRE(r->subData.size() == 0);
}

static constexpr auto methods_external_puml =
    R"(@startuml
Object <|-- ArrayList

Object : equals()
ArrayList : Object[] elementData
ArrayList : size() : int
@enduml)";

TEST_CASE("methods declared externally parsed", "[Parser]")
{
    Parser parser;
    PlantUMLPtr r = parser.parse(methods_external_puml);

    REQUIRE(r->subData.size() == 2);
    REQUIRE(r->subData[0]->type == PlantUML::Type::Class);
    REQUIRE(r->subData[0]->name == "ArrayList");
    REQUIRE(r->subData[1]->type == PlantUML::Type::Class);
    REQUIRE(r->subData[1]->name == "Object");

    REQUIRE(r->subData[0]->subData.size() == 2);
    REQUIRE(r->subData[0]->subData[0]->type == PlantUML::Type::Field);
    REQUIRE(r->subData[0]->subData[0]->name == "elementData");
    REQUIRE(r->subData[0]->subData[0]->valueType == "Object[]");
    REQUIRE(r->subData[0]->subData[1]->type == PlantUML::Type::Method);
    REQUIRE(r->subData[0]->subData[1]->name == "size");
    REQUIRE(r->subData[0]->subData[1]->valueType == "int");

    REQUIRE(r->subData[1]->subData.size() == 1);
    REQUIRE(r->subData[1]->subData[0]->type == PlantUML::Type::Method);
    REQUIRE(r->subData[1]->subData[0]->name == "equals");
    REQUIRE(r->subData[1]->subData[0]->valueType == "void");
}

static constexpr auto packages_puml =
    R"(@startuml
package "Classic Collections" #DDDDDD {
  Object <|-- ArrayList
}

package "net.sourceforge.plantuml" {
  Object <|-- Demo1
  Demo1 *- Demo2
}
@enduml)";

TEST_CASE("packages are parsed", "[Parser]")
{
    Parser parser;
    PlantUMLPtr r = parser.parse(packages_puml);

    REQUIRE(r->subData.size() == 2);
    REQUIRE(r->subData[0]->name == "Classic Collections");
    REQUIRE(r->subData[0]->type == PlantUML::Type::Package);
    REQUIRE(r->subData[1]->name == "net.sourceforge.plantuml");
    REQUIRE(r->subData[1]->type == PlantUML::Type::Package);

    REQUIRE(r->subData[0]->subData.size() == 2);
    REQUIRE(r->subData[0]->subData[0]->name == "ArrayList");
    REQUIRE(r->subData[0]->subData[1]->name == "Object");
    REQUIRE(r->subData[1]->subData.size() == 2);
    REQUIRE(r->subData[1]->subData[0]->name == "Demo1");
    REQUIRE(r->subData[1]->subData[1]->name == "Demo2");
}

static constexpr auto include_puml =
    R"(@startuml
!include something.iuml
@enduml)";

TEST_CASE("include in diagram is ignored", "[Parser]")
{
    Parser parser;
    PlantUMLPtr r = parser.parse(include_puml);

    REQUIRE(r->subData.size() == 0);
}

static constexpr auto namespaces_puml =
    R"(@startuml

class BaseClass

namespace "net.dummy" #DDDDDD {
    .BaseClass <|-- Person
    Meeting o-- Person

    .BaseClass <|- Meeting
}

namespace "net.foo" {
  net.dummy.Person  <|- Person
  .BaseClass <|-- Person

  net.dummy.Meeting o-- Person
}

BaseClass <|-- net.unused.Person

@enduml)";

TEST_CASE("Namespaces and namespaced identifiers are parsed", "[Parser]")
{
    Parser parser;
    PlantUMLPtr r = parser.parse(namespaces_puml);

    REQUIRE(r->subData.size() == 4);
    REQUIRE(r->findChild("BaseClass"));
    REQUIRE(r->findChild("BaseClass")->type == PlantUML::Type::Class);

    REQUIRE(r->findChild("net.dummy"));
    REQUIRE(r->findChild("net.dummy")->type == PlantUML::Type::Namespace);
    REQUIRE(r->findChild("net.dummy")->subData.size() == 2);
    REQUIRE(r->findChild("net.dummy")->findChild("Meeting"));
    REQUIRE(r->findChild("net.dummy")->findChild("Person"));

    REQUIRE(r->findChild("net.foo"));
    REQUIRE(r->findChild("net.foo")->type == PlantUML::Type::Namespace);
    REQUIRE(r->findChild("net.foo")->subData.size() == 1);
    REQUIRE(r->findChild("net.foo")->findChild("Person"));

    REQUIRE(r->findChild("net.unused"));
    REQUIRE(r->findChild("net.unused")->type == PlantUML::Type::Namespace);
    REQUIRE(r->findChild("net.unused")->subData.size() == 1);
    REQUIRE(r->findChild("net.unused")->findChild("Person"));
}

static constexpr auto namespace_separator_puml =
    R"(@startuml

set namespaceSeparator ::
class X1::X2::foo {
  some info
}

@enduml)";

TEST_CASE("Namespace separator is parsed", "[Parser]")
{
    Parser parser;
    PlantUMLPtr r = parser.parse(namespace_separator_puml);

    REQUIRE(r->subData.size() == 1);

    REQUIRE(r->findChild("X1::X2"));
    REQUIRE(r->findChild("X1::X2")->type == PlantUML::Type::Namespace);
    REQUIRE(r->findChild("X1::X2")->subData.size() == 1);
    REQUIRE(r->findChild("X1::X2")->findChild("foo"));
}

static constexpr auto enum_with_body_puml =
    R"(@startuml
enum TimeUnit {
  DAYS
  HOURS
  MINUTES
}

@enduml)";

TEST_CASE("enum with body is parsed", "[Parser]")
{
    Parser parser;
    PlantUMLPtr r = parser.parse(enum_with_body_puml);

    REQUIRE(r->subData.size() == 1);
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

TEST_CASE("class with stereotype and custom spot is parsed", "[Parser]")
{
    Parser parser;
    PlantUMLPtr r = parser.parse(class_with_stereotype_and_spot_puml);

    REQUIRE(r->subData.size() == 3);
}

static constexpr auto hidden_relationships_puml =
    R"(@startuml

Together2 -[hidden]--> Bar1
Bar1 -[hidden]> Bar2

@enduml)";

TEST_CASE("hidden relationships are parsed", "[Parser]")
{
    Parser parser;
    PlantUMLPtr r = parser.parse(hidden_relationships_puml);

    REQUIRE(r->subData.size() == 3);
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

TEST_CASE("real live example part 1 is parsed", "[Parser]")
{
    Parser parser;
    PlantUMLPtr r = parser.parse(real_live_example_part_1_puml);

    REQUIRE(r->subData.size() == 1);
    REQUIRE(r->subData[0]->name == "data");
    REQUIRE(r->subData[0]->subData.size() == 13);
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

TEST_CASE("real live example part 2 is parsed", "[Parser]")
{
    Parser parser;
    PlantUMLPtr r = parser.parse(real_live_example_part_2_puml);

    REQUIRE(r->subData.size() == 1);
    REQUIRE(r->subData[0]->name == "algorithms");
    REQUIRE(r->subData[0]->subData.size() == 11);
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

TEST_CASE("real live example part 3 is parsed", "[Parser]")
{
    Parser parser;
    PlantUMLPtr r = parser.parse(real_live_example_part_3_puml);

    REQUIRE(r->subData.size() == 24);
}