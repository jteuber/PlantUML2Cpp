#include "gtest/gtest.h"

#include "Cpp/Class/Translator.h"
#include "PlantUml/Parser.h"
#include "spdlog/spdlog.h"

namespace puml = PlantUml;

namespace Cpp::Class {

auto act(std::string_view puml)
{
    spdlog::set_level(spdlog::level::trace);
    Translator sut{std::make_shared<Config>()};
    PlantUml::Parser parser;
    EXPECT_TRUE(parser.parse(puml));
    parser.getAST().visit(sut);

    return std::move(sut).results();
}

TEST(ClassTranslatorTest, EmptyDiagram)
{
    // Arrange
    Translator sut{std::make_shared<Config>()};

    puml::Container c{{}, "", puml::ContainerType::Document};
    puml::End ec{puml::EndType::Document};

    // Act
    sut.visit(c);
    sut.visit(ec);

    // Assert
    auto classes = std::move(sut).results();
    EXPECT_TRUE(classes.empty());
}

TEST(ClassTranslatorTest, SingleAbstract)
{
    // Arrange
    Translator sut{std::make_shared<Config>()};

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
    auto classes = std::move(sut).results();
    ASSERT_EQ(classes.size(), 1);

    EXPECT_EQ(classes[0].name, "name");
    EXPECT_EQ(classes[0].isInterface, false);
    EXPECT_EQ(classes[0].isStruct, false);
    EXPECT_TRUE(classes[0].namespaces.empty());
}

TEST(ClassTranslatorTest, AllSimpleContainers)
{
    // Arrange
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
    auto classes = act(puml);

    // Assert
    ASSERT_EQ(classes.size(), 4);

    EXPECT_EQ(classes[0].name, "abstract");
    EXPECT_EQ(classes[0].isInterface, false);
    EXPECT_EQ(classes[0].isStruct, false);
    EXPECT_EQ(classes[1].name, "class");
    EXPECT_EQ(classes[1].isInterface, false);
    EXPECT_EQ(classes[1].isStruct, false);
    EXPECT_EQ(classes[2].name, "entity");
    EXPECT_EQ(classes[2].isInterface, false);
    EXPECT_EQ(classes[2].isStruct, true);
    EXPECT_EQ(classes[3].name, "interface");
    EXPECT_EQ(classes[3].isInterface, true);
    EXPECT_EQ(classes[3].isStruct, false);
}

TEST(ClassTranslatorTest, EntityWithEmptyBody)
{
    // Arrange
    static constexpr auto puml =
        R"(@startuml
    entity test { }
    @enduml)";

    // Act
    auto classes = act(puml);

    // Assert
    ASSERT_EQ(classes.size(), 1);

    EXPECT_EQ(classes[0].name, "test");
    EXPECT_EQ(classes[0].isInterface, false);
    EXPECT_EQ(classes[0].isStruct, true);
    EXPECT_TRUE(classes[0].body.empty());
}

TEST(ClassTranslatorTest, EntityWithSingleVariable)
{
    // Arrange
    static constexpr auto puml =
        R"(@startuml
    entity test {
        variable : var
    }
    @enduml)";

    // Act
    auto classes = act(puml);

    // Assert
    ASSERT_EQ(classes.size(), 1);

    EXPECT_EQ(classes[0].name, "test");
    EXPECT_EQ(classes[0].isInterface, false);
    EXPECT_EQ(classes[0].isStruct, true);

    ASSERT_EQ(classes[0].body.size(), 1);
    EXPECT_EQ(std::get<Variable>(classes[0].body[0]).name, "variable");
    EXPECT_EQ(std::get<Variable>(classes[0].body[0]).type, Common::Type{"var"});
}

TEST(ClassTranslatorTest, ClassWithTwoMethods)
{
    // Arrange
    static constexpr auto puml =
        R"(@startuml
        class test {
            method() : var
            method2(input : bool) : var
        }
        @enduml)";

    // Act
    auto classes = act(puml);

    // Assert
    ASSERT_EQ(classes.size(), 1);

    EXPECT_EQ(classes[0].name, "test");

    ASSERT_EQ(classes[0].body.size(), 2);
    EXPECT_EQ(std::get<Method>(classes[0].body[0]).name, "method");
    EXPECT_EQ(std::get<Method>(classes[0].body[0]).returnType, Common::Type{"var"});
    EXPECT_TRUE(std::get<Method>(classes[0].body[0]).parameters.empty());

    EXPECT_EQ(std::get<Method>(classes[0].body[1]).name, "method2");
    EXPECT_EQ(std::get<Method>(classes[0].body[1]).returnType, Common::Type{"var"});

    ASSERT_EQ(std::get<Method>(classes[0].body[1]).parameters.size(), 1);
    EXPECT_EQ(std::get<Method>(classes[0].body[1]).parameters[0].name, "input");
    EXPECT_EQ(std::get<Method>(classes[0].body[1]).parameters[0].type, Common::Type{"bool"});
}

TEST(ClassTranslatorTest, Inheritance)
{
    // Arrange
    static constexpr auto puml =
        R"(@startuml
        class Class02
        Class01 <|-- Class02
        @enduml)";

    // Act
    auto classes = act(puml);

    // Assert
    ASSERT_EQ(classes.size(), 1);

    EXPECT_EQ(classes[0].name, "Class02");
    ASSERT_EQ(classes[0].inherits.size(), 1);
    EXPECT_EQ(classes[0].inherits[0], "Class01");
}

TEST(ClassTranslatorTest, Composition)
{
    // Arrange
    static constexpr auto puml =
        R"(@startuml
        class Class03
        Class03 *-- Class04
        @enduml)";

    // Act
    auto classes = act(puml);

    // Assert
    ASSERT_EQ(classes.size(), 1);

    EXPECT_EQ(classes[0].name, "Class03");
    ASSERT_EQ(classes[0].body.size(), 1);
    EXPECT_EQ(std::get<Variable>(classes[0].body[0]).name, "class04");
    EXPECT_EQ(std::get<Variable>(classes[0].body[0]).type, Common::Type{"Class04"});
}

TEST(ClassTranslatorTest, Aggregation)
{
    // Arrange
    static constexpr auto puml =
        R"(@startuml
        class Class05
        Class05 o-- Class06
        @enduml)";

    // Act
    auto classes = act(puml);

    // Assert
    ASSERT_EQ(classes.size(), 1);

    EXPECT_EQ(classes[0].name, "Class05");
    ASSERT_EQ(classes[0].body.size(), 1);
    EXPECT_EQ(std::get<Variable>(classes[0].body[0]).name, "class06");
    EXPECT_EQ(std::get<Variable>(classes[0].body[0]).type, (Common::Type{"std::shared_ptr", {{"Class06"}}}));
}

TEST(ClassTranslatorTest, LabelsOnRelations)
{
    // Arrange
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
    auto classes = act(puml);

    // Assert
    ASSERT_EQ(classes.size(), 4);

    EXPECT_EQ(classes[0].name, "Class01");
    EXPECT_EQ(classes[0].isInterface, false);
    EXPECT_EQ(classes[0].isStruct, false);
    ASSERT_EQ(classes[0].body.size(), 1);
    EXPECT_EQ(std::get<Variable>(classes[0].body[0]).name, "contains");
    EXPECT_EQ(std::get<Variable>(classes[0].body[0]).type, (Common::Type{"std::vector", {{"Class02"}}}));

    EXPECT_EQ(classes[2].name, "Class03");
    EXPECT_EQ(classes[2].isInterface, false);
    EXPECT_EQ(classes[2].isStruct, false);
    ASSERT_EQ(classes[2].body.size(), 1);
    EXPECT_EQ(std::get<Variable>(classes[2].body[0]).name, "aggregation");
    EXPECT_EQ(std::get<Variable>(classes[2].body[0]).type, (Common::Type{"std::shared_ptr", {{"Class04"}}}));
}

TEST(ClassTranslatorTest, ExternalMethodsAndVariables)
{
    // Arrange
    static constexpr auto puml =
        R"(@startuml
        class Object
        class ArrayList

        Object : equals()
        ArrayList : Object[] elementData
        ArrayList : size() : int
        @enduml)";

    // Act
    auto classes = act(puml);

    // Assert
    ASSERT_EQ(classes.size(), 2);

    EXPECT_EQ(classes[0].name, "Object");
    ASSERT_EQ(classes[0].body.size(), 1);
    EXPECT_EQ(std::get<Method>(classes[0].body[0]).name, "equals");
    EXPECT_EQ(std::get<Method>(classes[0].body[0]).returnType, Common::Type{"void"});
    EXPECT_TRUE(std::get<Method>(classes[0].body[0]).parameters.empty());

    EXPECT_EQ(classes[1].name, "ArrayList");
    ASSERT_EQ(classes[1].body.size(), 2);

    EXPECT_EQ(std::get<Variable>(classes[1].body[0]).name, "elementData");
    EXPECT_EQ(std::get<Variable>(classes[1].body[0]).type, Common::Type{"Object[]"});
    EXPECT_EQ(std::get<Method>(classes[1].body[1]).name, "size");
    EXPECT_EQ(std::get<Method>(classes[1].body[1]).returnType, Common::Type{"int"});
    EXPECT_TRUE(std::get<Method>(classes[1].body[1]).parameters.empty());
}

TEST(ClassTranslatorTest, Namespaces)
{
    // Arrange
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
    auto classes = act(puml);

    // Assert
    ASSERT_EQ(classes.size(), 3);

    EXPECT_EQ(classes[0].name, "BaseClass");
    EXPECT_TRUE(classes[0].namespaces.empty());

    Class person = classes[1];
    EXPECT_EQ(person.name, "Person");
    ASSERT_EQ(person.namespaces.size(), 2);
    EXPECT_EQ(person.namespaces.back(), "dummy");
    person.namespaces.pop_back();
    EXPECT_EQ(person.namespaces.back(), "net");
    ASSERT_EQ(person.inherits.size(), 1);
    EXPECT_EQ(person.inherits[0], "::BaseClass");

    EXPECT_EQ(classes[2].name, "BaseClass2");
    EXPECT_TRUE(classes[2].namespaces.empty());
}

TEST(ClassTranslatorTest, NamespaceSeperator)
{
    // Arrange
    static constexpr auto puml =
        R"(@startuml

        set namespaceSeparator ::
        class X1::X2::foo {
        some info
        }

        @enduml)";

    // Act
    auto classes = act(puml);

    // Assert
    ASSERT_EQ(classes.size(), 1);

    Class foo = classes[0];
    EXPECT_EQ(foo.name, "foo");
    ASSERT_EQ(foo.namespaces.size(), 2);
    EXPECT_EQ(foo.namespaces.back(), "X2");
    foo.namespaces.pop_back();
    EXPECT_EQ(foo.namespaces.back(), "X1");
}

TEST(ClassTranslatorTest, NestedNamespaces)
{
    // Arrange
    static constexpr auto puml =
        R"(@startuml

        namespace net {
            namespace dummy {
                class Person
            }
        }

        @enduml)";

    // Act
    auto classes = act(puml);

    // Assert
    ASSERT_EQ(classes.size(), 1);

    Class foo = classes[0];
    EXPECT_EQ(foo.name, "Person");
    ASSERT_EQ(foo.namespaces.size(), 2);
    EXPECT_EQ(foo.namespaces.back(), "dummy");
    foo.namespaces.pop_back();
    EXPECT_EQ(foo.namespaces.back(), "net");
}

TEST(ClassTranslatorTest, InheritFromOtherNamespace)
{
    // Arrange
    static constexpr auto puml =
        R"(@startuml

        namespace net {
            class foo
        }
        namespace dummy {
            class bar
            bar --|> net.foo
        }

        @enduml)";

    // Act
    auto classes = act(puml);

    // Assert
    ASSERT_EQ(classes.size(), 2);

    Class foo = classes[0];
    EXPECT_EQ(foo.name, "foo");
    ASSERT_EQ(foo.namespaces.size(), 1);
    EXPECT_EQ(foo.namespaces.back(), "net");

    Class bar = classes[1];
    EXPECT_EQ(bar.name, "bar");
    ASSERT_EQ(bar.inherits.size(), 1);
    EXPECT_EQ(bar.inherits[0], "net::foo");
    ASSERT_EQ(bar.namespaces.size(), 1);
    EXPECT_EQ(bar.namespaces.back(), "dummy");
}

TEST(ClassTranslatorTest, Stereotypes)
{
    // Arrange
    static constexpr auto puml =
        R"(@startuml

        class SystemData << (S,#FF7700) Struct >>

        @enduml)";

    // Act
    auto classes = act(puml);

    // Assert
    ASSERT_EQ(classes.size(), 1);

    Class system = classes[0];
    EXPECT_EQ(system.name, "SystemData");
    EXPECT_TRUE(system.isStruct);
}

TEST(ClassTranslatorTest, Templates)
{
    // Arrange
    Translator sut{std::make_shared<Config>()};

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
    auto classes = std::move(sut).results();
    ASSERT_EQ(classes.size(), 1);

    EXPECT_EQ(classes[0].name, "test");
    EXPECT_EQ(classes[0].isInterface, false);
    EXPECT_EQ(classes[0].isStruct, false);

    ASSERT_EQ(classes[0].body.size(), 2);
    EXPECT_EQ(std::get<Variable>(classes[0].body[0]).name, "variable");
    EXPECT_EQ(std::get<Variable>(classes[0].body[0]).type,
              (Common::Type{"std::vector", {Common::Type{"std::string"}}}));

    EXPECT_EQ(std::get<Variable>(classes[0].body[1]).name, "map");
    EXPECT_EQ(std::get<Variable>(classes[0].body[1]).type,
              (Common::Type{"std::map", {Common::Type{"std::string"}, Common::Type{"int"}}}));
}

TEST(ClassTranslatorTest, Interface)
{
    // Arrange
    static constexpr auto puml =
        R"(@startuml
    interface test {
        method() : var
        method2(input : bool) : var
    }
    @enduml)";

    // Act
    auto classes = act(puml);

    // Assert
    ASSERT_EQ(classes.size(), 1);

    EXPECT_EQ(classes[0].name, "test");

    ASSERT_EQ(classes[0].body.size(), 2);
    EXPECT_EQ(std::get<Method>(classes[0].body[0]).name, "method");
    EXPECT_EQ(std::get<Method>(classes[0].body[0]).returnType, Common::Type{"var"});
    EXPECT_TRUE(std::get<Method>(classes[0].body[0]).isAbstract);
    EXPECT_TRUE(std::get<Method>(classes[0].body[0]).parameters.empty());

    EXPECT_EQ(std::get<Method>(classes[0].body[1]).name, "method2");
    EXPECT_EQ(std::get<Method>(classes[0].body[1]).returnType, Common::Type{"var"});
    EXPECT_TRUE(std::get<Method>(classes[0].body[1]).isAbstract);

    ASSERT_EQ(std::get<Method>(classes[0].body[1]).parameters.size(), 1);
    EXPECT_EQ(std::get<Method>(classes[0].body[1]).parameters[0].name, "input");
    EXPECT_EQ(std::get<Method>(classes[0].body[1]).parameters[0].type, Common::Type{"bool"});
}

TEST(ClassTranslatorTest, Visibility)
{
    // Arrange
    static constexpr auto puml =
        R"(@startuml
    class test {
        +method() : var
        -method2(input : bool) : var
    }
    @enduml)";

    // Act
    auto classes = act(puml);

    // Assert
    ASSERT_EQ(classes.size(), 1);

    EXPECT_EQ(classes[0].name, "test");

    ASSERT_EQ(classes[0].body.size(), 4);
    EXPECT_EQ(std::get<VisibilityKeyword>(classes[0].body[0]).name, "public:");
    EXPECT_EQ(std::get<Method>(classes[0].body[1]).name, "method");

    EXPECT_EQ(std::get<VisibilityKeyword>(classes[0].body[2]).name, "private:");
    EXPECT_EQ(std::get<Method>(classes[0].body[3]).name, "method2");
}

TEST(ClassTranslatorTest, SameClassNameDifferentNamespace)
{
    // Arrange
    static constexpr auto puml =
        R"(@startuml
        class BaseClass
        namespace Adam {
            class Person
        }
        namespace Steve {
            class Person
            Person --|> .BaseClass
        }

        @enduml)";

    // Act
    auto classes = act(puml);

    // Assert
    ASSERT_EQ(classes.size(), 3);

    EXPECT_EQ(classes[0].name, "BaseClass");
    EXPECT_TRUE(classes[0].namespaces.empty());

    Class person = classes[2];
    EXPECT_EQ(person.name, "Person");
    EXPECT_EQ(person.namespaces.size(), 1);
    EXPECT_EQ(person.namespaces.back(), "Steve");
    ASSERT_EQ(person.inherits.size(), 1);
    EXPECT_EQ(person.inherits[0], "::BaseClass");
}

TEST(ClassTranslatorTest, ClassWithSpotSIsStruct)
{
    // Arrange
    static constexpr auto puml =
        R"(@startuml

        class Common::Type << (S,#FFAA55) >>
        {
            +base : list<string>
            +templateParams : vector<Common::Type>
        }

        class Singleton << (S,#FFAA55) Singleton >>

        @enduml)";

    // Act
    auto classes = act(puml);

    // Assert
    ASSERT_EQ(classes.size(), 2);

    Class type = classes[0];
    EXPECT_EQ(type.name, "Common::Type");
    ASSERT_TRUE(type.isStruct);
}

TEST(ClassTranslatorTest, ClassWithSpotSButStereotypeNotStructIsNotStruct)
{
    // Arrange
    static constexpr auto puml =
        R"(@startuml
        class Singleton << (S,#FFAA55) Singleton >>

        @enduml)";

    // Act
    auto classes = act(puml);

    // Assert
    ASSERT_EQ(classes.size(), 1);

    Class type = classes[0];
    EXPECT_EQ(type.name, "Singleton");
    ASSERT_FALSE(type.isStruct);
}

TEST(ClassTranslatorTest, ClassWithSpotSAndStereotypeStructIsStruct)
{
    // Arrange
    static constexpr auto puml =
        R"(@startuml
        class Struct << (S,#FFAA55) Struct >>

        @enduml)";

    // Act
    auto classes = act(puml);

    // Assert
    ASSERT_EQ(classes.size(), 1);

    Class type = classes[0];
    EXPECT_EQ(type.name, "Struct");
    ASSERT_TRUE(type.isStruct);
}

} // namespace Cpp::Class
