#include "gtest/gtest.h"

#include "Cpp/ClassTranslator.h"
#include "PlantUml/Parser.h"

namespace puml = PlantUml;

void act(PlantUml::Parser& parser, Cpp::ClassTranslator& sut, std::string_view puml)
{
    ASSERT_TRUE(parser.parse(puml));
    parser.visitAST(sut);
}

TEST(ClassTranslatorTest, EmptyDiagram)
{
    // Arrange
    Cpp::ClassTranslator sut{std::make_shared<Config>()};

    puml::Container c{{}, "", puml::ContainerType::Document};
    puml::End ec{puml::EndType::Document};

    // Act
    sut.visit(c);
    sut.visit(ec);

    // Assert
    EXPECT_TRUE(sut.results().empty());
}

TEST(ClassTranslatorTest, SingleAbstract)
{
    // Arrange
    Cpp::ClassTranslator sut{std::make_shared<Config>()};

    puml::Container c{{}, "", puml::ContainerType::Document};
    puml::Element e{{"name"}, "", ' ', {}, {}, puml::ElementType::Abstract};
    puml::End ee{puml::EndType::Element};
    puml::End ec{puml::EndType::Document};

    // Act
    sut.visit(c);
    sut.visit(e);
    sut.visit(ee);
    sut.visit(ec);

    // Assert
    ASSERT_EQ(sut.results().size(), 1);

    EXPECT_EQ(sut.results()[0].name, "name");
    EXPECT_EQ(sut.results()[0].isInterface, false);
    EXPECT_EQ(sut.results()[0].isStruct, false);
    EXPECT_TRUE(sut.results()[0].namespaces.empty());
}

TEST(ClassTranslatorTest, AllSimpleContainers)
{
    // Arrange
    Cpp::ClassTranslator sut{std::make_shared<Config>()};
    PlantUml::Parser parser;

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
    EXPECT_EQ(sut.results()[0].isInterface, false);
    EXPECT_EQ(sut.results()[0].isStruct, false);
    EXPECT_EQ(sut.results()[1].name, "class");
    EXPECT_EQ(sut.results()[1].isInterface, false);
    EXPECT_EQ(sut.results()[1].isStruct, false);
    EXPECT_EQ(sut.results()[2].name, "entity");
    EXPECT_EQ(sut.results()[2].isInterface, false);
    EXPECT_EQ(sut.results()[2].isStruct, true);
    EXPECT_EQ(sut.results()[3].name, "interface");
    EXPECT_EQ(sut.results()[3].isInterface, true);
    EXPECT_EQ(sut.results()[3].isStruct, false);
}

TEST(ClassTranslatorTest, EntityWithEmptyBody)
{
    // Arrange
    Cpp::ClassTranslator sut{std::make_shared<Config>()};
    PlantUml::Parser parser;

    static constexpr auto puml =
        R"(@startuml
    entity test { }
    @enduml)";

    // Act
    act(parser, sut, puml);

    // Assert
    ASSERT_EQ(sut.results().size(), 1);

    EXPECT_EQ(sut.results()[0].name, "test");
    EXPECT_EQ(sut.results()[0].isInterface, false);
    EXPECT_EQ(sut.results()[0].isStruct, true);
    EXPECT_TRUE(sut.results()[0].body.empty());
}

TEST(ClassTranslatorTest, EntityWithSingleVariable)
{
    // Arrange
    Cpp::ClassTranslator sut{std::make_shared<Config>()};
    PlantUml::Parser parser;

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
    EXPECT_EQ(sut.results()[0].isInterface, false);
    EXPECT_EQ(sut.results()[0].isStruct, true);

    ASSERT_EQ(sut.results()[0].body.size(), 1);
    EXPECT_EQ(std::get<Cpp::Variable>(sut.results()[0].body[0]).name, "variable");
    EXPECT_EQ(std::get<Cpp::Variable>(sut.results()[0].body[0]).type, Cpp::Type{"var"});
}

TEST(ClassTranslatorTest, ClassWithTwoMethods)
{
    // Arrange
    Cpp::ClassTranslator sut{std::make_shared<Config>()};
    PlantUml::Parser parser;

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

    ASSERT_EQ(sut.results()[0].body.size(), 2);
    EXPECT_EQ(std::get<Cpp::Method>(sut.results()[0].body[0]).name, "method");
    EXPECT_EQ(std::get<Cpp::Method>(sut.results()[0].body[0]).returnType, Cpp::Type{"var"});
    EXPECT_TRUE(std::get<Cpp::Method>(sut.results()[0].body[0]).parameters.empty());

    EXPECT_EQ(std::get<Cpp::Method>(sut.results()[0].body[1]).name, "method2");
    EXPECT_EQ(std::get<Cpp::Method>(sut.results()[0].body[1]).returnType, Cpp::Type{"var"});

    ASSERT_EQ(std::get<Cpp::Method>(sut.results()[0].body[1]).parameters.size(), 1);
    EXPECT_EQ(std::get<Cpp::Method>(sut.results()[0].body[1]).parameters[0].name, "input");
    EXPECT_EQ(std::get<Cpp::Method>(sut.results()[0].body[1]).parameters[0].type, Cpp::Type{"bool"});
}

TEST(ClassTranslatorTest, Inheritance)
{
    // Arrange
    Cpp::ClassTranslator sut{std::make_shared<Config>()};
    PlantUml::Parser parser;

    static constexpr auto puml =
        R"(@startuml
        class Class02
        Class01 <|-- Class02
        @enduml)";

    // Act
    act(parser, sut, puml);

    // Assert
    ASSERT_EQ(sut.results().size(), 1);

    EXPECT_EQ(sut.results()[0].name, "Class02");
    ASSERT_EQ(sut.results()[0].inherits.size(), 1);
    EXPECT_EQ(sut.results()[0].inherits[0], "Class01");
}

TEST(ClassTranslatorTest, Composition)
{
    // Arrange
    Cpp::ClassTranslator sut{std::make_shared<Config>()};
    PlantUml::Parser parser;

    static constexpr auto puml =
        R"(@startuml
        class Class03
        Class03 *-- Class04
        @enduml)";

    // Act
    act(parser, sut, puml);

    // Assert
    ASSERT_EQ(sut.results().size(), 1);

    EXPECT_EQ(sut.results()[0].name, "Class03");
    ASSERT_EQ(sut.results()[0].body.size(), 1);
    EXPECT_EQ(std::get<Cpp::Variable>(sut.results()[0].body[0]).name, "");
    EXPECT_EQ(std::get<Cpp::Variable>(sut.results()[0].body[0]).type, Cpp::Type{"Class04"});
}

TEST(ClassTranslatorTest, Aggregation)
{
    // Arrange
    Cpp::ClassTranslator sut{std::make_shared<Config>()};
    PlantUml::Parser parser;

    static constexpr auto puml =
        R"(@startuml
        class Class05
        Class05 o-- Class06
        @enduml)";

    // Act
    act(parser, sut, puml);

    // Assert
    ASSERT_EQ(sut.results().size(), 1);

    EXPECT_EQ(sut.results()[0].name, "Class05");
    ASSERT_EQ(sut.results()[0].body.size(), 1);
    EXPECT_EQ(std::get<Cpp::Variable>(sut.results()[0].body[0]).name, "");
    EXPECT_EQ(std::get<Cpp::Variable>(sut.results()[0].body[0]).type, Cpp::Type{"std::shared_ptr<Class06>"});
}

TEST(ClassTranslatorTest, LabelsOnRelations)
{
    // Arrange
    Cpp::ClassTranslator sut{std::make_shared<Config>()};
    PlantUml::Parser parser;

    static constexpr auto puml =
        R"(@startuml
        class Class01
        class Class02
        class Class03
        class Class04

        Class01 "1" *-- "0..*" Class02 : contains
        Class03 o-- Class04 : aggregation
        @enduml)";

    // Act
    act(parser, sut, puml);

    // Assert
    ASSERT_EQ(sut.results().size(), 4);

    EXPECT_EQ(sut.results()[0].name, "Class01");
    EXPECT_EQ(sut.results()[0].isInterface, false);
    EXPECT_EQ(sut.results()[0].isStruct, false);
    ASSERT_EQ(sut.results()[0].body.size(), 1);
    EXPECT_EQ(std::get<Cpp::Variable>(sut.results()[0].body[0]).name, "contains");
    EXPECT_EQ(std::get<Cpp::Variable>(sut.results()[0].body[0]).type, Cpp::Type{"std::vector<Class02>"});

    EXPECT_EQ(sut.results()[2].name, "Class03");
    EXPECT_EQ(sut.results()[2].isInterface, false);
    EXPECT_EQ(sut.results()[2].isStruct, false);
    ASSERT_EQ(sut.results()[2].body.size(), 1);
    EXPECT_EQ(std::get<Cpp::Variable>(sut.results()[2].body[0]).name, "aggregation");
    EXPECT_EQ(std::get<Cpp::Variable>(sut.results()[2].body[0]).type, Cpp::Type{"std::shared_ptr<Class04>"});
}

TEST(ClassTranslatorTest, ExternalMethodsAndVariables)
{
    // Arrange
    Cpp::ClassTranslator sut{std::make_shared<Config>()};
    PlantUml::Parser parser;

    static constexpr auto puml =
        R"(@startuml
        class Object
        class ArrayList

        Object : equals()
        ArrayList : Object[] elementData
        ArrayList : size() : int
        @enduml)";

    // Act
    act(parser, sut, puml);

    // Assert
    ASSERT_EQ(sut.results().size(), 2);

    EXPECT_EQ(sut.results()[0].name, "Object");
    ASSERT_EQ(sut.results()[0].body.size(), 1);
    EXPECT_EQ(std::get<Cpp::Method>(sut.results()[0].body[0]).name, "equals");
    EXPECT_EQ(std::get<Cpp::Method>(sut.results()[0].body[0]).returnType, Cpp::Type{"void"});
    EXPECT_TRUE(std::get<Cpp::Method>(sut.results()[0].body[0]).parameters.empty());

    EXPECT_EQ(sut.results()[1].name, "ArrayList");
    ASSERT_EQ(sut.results()[1].body.size(), 2);

    EXPECT_EQ(std::get<Cpp::Variable>(sut.results()[1].body[0]).name, "elementData");
    EXPECT_EQ(std::get<Cpp::Variable>(sut.results()[1].body[0]).type, Cpp::Type{"Object[]"});
    EXPECT_EQ(std::get<Cpp::Method>(sut.results()[1].body[1]).name, "size");
    EXPECT_EQ(std::get<Cpp::Method>(sut.results()[1].body[1]).returnType, Cpp::Type{"int"});
    EXPECT_TRUE(std::get<Cpp::Method>(sut.results()[1].body[1]).parameters.empty());
}

TEST(ClassTranslatorTest, Namespaces)
{
    // Arrange
    Cpp::ClassTranslator sut{std::make_shared<Config>()};
    PlantUml::Parser parser;

    static constexpr auto puml =
        R"(@startuml

        class BaseClass
        namespace net.dummy #DDDDDD {
            class Person
            .BaseClass <|-- Person
        }
        class BaseClass2

        @enduml)";

    // Act
    act(parser, sut, puml);

    // Assert
    ASSERT_EQ(sut.results().size(), 3);

    EXPECT_EQ(sut.results()[0].name, "BaseClass");
    EXPECT_TRUE(sut.results()[0].namespaces.empty());

    Cpp::Class person = sut.results()[1];
    EXPECT_EQ(person.name, "Person");
    ASSERT_EQ(person.namespaces.size(), 2);
    EXPECT_EQ(person.namespaces.back(), "dummy");
    person.namespaces.pop_back();
    EXPECT_EQ(person.namespaces.back(), "net");
    ASSERT_EQ(person.inherits.size(), 1);
    EXPECT_EQ(person.inherits[0], "BaseClass");

    EXPECT_EQ(sut.results()[2].name, "BaseClass2");
    EXPECT_TRUE(sut.results()[2].namespaces.empty());
}

TEST(ClassTranslatorTest, NamespaceSeperator)
{
    // Arrange
    Cpp::ClassTranslator sut{std::make_shared<Config>()};
    PlantUml::Parser parser;

    static constexpr auto puml =
        R"(@startuml

        set namespaceSeparator ::
        class X1::X2::foo {
        some info
        }

        @enduml)";

    // Act
    act(parser, sut, puml);

    // Assert
    ASSERT_EQ(sut.results().size(), 1);

    Cpp::Class foo = sut.results()[0];
    EXPECT_EQ(foo.name, "foo");
    ASSERT_EQ(foo.namespaces.size(), 2);
    EXPECT_EQ(foo.namespaces.back(), "X2");
    foo.namespaces.pop_back();
    EXPECT_EQ(foo.namespaces.back(), "X1");
}

TEST(ClassTranslatorTest, Stereotypes)
{
    // Arrange
    Cpp::ClassTranslator sut{std::make_shared<Config>()};
    PlantUml::Parser parser;

    static constexpr auto puml =
        R"(@startuml

        class SystemData << (S,#FF7700) Struct >>

        @enduml)";

    // Act
    act(parser, sut, puml);

    // Assert
    ASSERT_EQ(sut.results().size(), 1);

    Cpp::Class system = sut.results()[0];
    EXPECT_EQ(system.name, "SystemData");
    EXPECT_TRUE(system.isStruct);
}

TEST(ClassTranslatorTest, Templates)
{
    // Arrange
    Cpp::ClassTranslator sut{std::make_shared<Config>()};

    puml::Element e{{"test"}, "", ' ', {}, {}, puml::ElementType::Class};
    puml::Variable v{
        "variable", {{"vector"}, {puml::Type{{"string"}}}}, {}, puml::Visibility::Unspecified, false, false};
    puml::Variable m{"map",
                     {{"std", "map"}, {puml::Type{{"std", "string"}}, puml::Type{{"int"}}}},
                     {},
                     puml::Visibility::Unspecified,
                     false,
                     false};
    puml::End ee{puml::EndType::Element};
    puml::End ec{puml::EndType::Document};

    // Act
    sut.visit(e);
    sut.visit(v);
    sut.visit(m);
    sut.visit(ee);
    sut.visit(ec);

    // Assert
    ASSERT_EQ(sut.results().size(), 1);

    EXPECT_EQ(sut.results()[0].name, "test");
    EXPECT_EQ(sut.results()[0].isInterface, false);
    EXPECT_EQ(sut.results()[0].isStruct, false);

    ASSERT_EQ(sut.results()[0].body.size(), 2);
    EXPECT_EQ(std::get<Cpp::Variable>(sut.results()[0].body[0]).name, "variable");
    EXPECT_EQ(std::get<Cpp::Variable>(sut.results()[0].body[0]).type,
              (Cpp::Type{"std::vector", {Cpp::Type{"std::string"}}}));

    EXPECT_EQ(std::get<Cpp::Variable>(sut.results()[0].body[1]).name, "map");
    EXPECT_EQ(std::get<Cpp::Variable>(sut.results()[0].body[1]).type,
              (Cpp::Type{"std::map", {Cpp::Type{"std::string"}, Cpp::Type{"int"}}}));
}
