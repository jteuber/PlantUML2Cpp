#include "gtest/gtest.h"

#include "Cpp/ClassBuilder.h"
#include "PlantUml/Parser.h"

namespace puml = PlantUml;

void act(PlantUml::Parser& parser, Cpp::ClassBuilder& sut, std::string_view puml)
{
    ASSERT_TRUE(parser.parse(puml));
    parser.visitAST(sut);
}

TEST(ClassBuilderTest, EmptyDiagram)
{
    // Arrange
    Cpp::ClassBuilder sut;

    puml::Container c{{}, "", puml::ContainerType::Document};
    puml::End ec{puml::EndType::Document};

    // Act
    sut.visit(c);
    sut.visit(ec);

    // Assert
    EXPECT_TRUE(sut.results().empty());
}

TEST(ClassBuilderTest, SingleAbstract)
{
    // Arrange
    Cpp::ClassBuilder sut;

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
    EXPECT_EQ(sut.results()[0].type, Cpp::Class::Type::Abstract);
    EXPECT_TRUE(sut.results()[0].namespaceStack.empty());
}

TEST(ClassBuilderTest, AllSimpleContainers)
{
    // Arrange
    Cpp::ClassBuilder sut;
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
    EXPECT_EQ(sut.results()[0].type, Cpp::Class::Type::Abstract);
    EXPECT_EQ(sut.results()[1].name, "class");
    EXPECT_EQ(sut.results()[1].type, Cpp::Class::Type::Class);
    EXPECT_EQ(sut.results()[2].name, "entity");
    EXPECT_EQ(sut.results()[2].type, Cpp::Class::Type::Struct);
    EXPECT_EQ(sut.results()[3].name, "interface");
    EXPECT_EQ(sut.results()[3].type, Cpp::Class::Type::Interface);
}

TEST(ClassBuilderTest, EntityWithEmptyBody)
{
    // Arrange
    Cpp::ClassBuilder sut;
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
    EXPECT_EQ(sut.results()[0].type, Cpp::Class::Type::Struct);
    EXPECT_TRUE(sut.results()[0].variables.empty());
    EXPECT_TRUE(sut.results()[0].methods.empty());
}

TEST(ClassBuilderTest, EntityWithSingleVariable)
{
    // Arrange
    Cpp::ClassBuilder sut;
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
    EXPECT_EQ(sut.results()[0].type, Cpp::Class::Type::Struct);
    EXPECT_TRUE(sut.results()[0].methods.empty());

    ASSERT_EQ(sut.results()[0].variables.size(), 1);
    EXPECT_EQ(sut.results()[0].variables[0].name, "variable");
    EXPECT_EQ(sut.results()[0].variables[0].type, "var");
}

TEST(ClassBuilderTest, ClassWithTwoMethods)
{
    // Arrange
    Cpp::ClassBuilder sut;
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
    EXPECT_EQ(sut.results()[0].type, Cpp::Class::Type::Class);
    EXPECT_TRUE(sut.results()[0].variables.empty());

    ASSERT_EQ(sut.results()[0].methods.size(), 2);
    EXPECT_EQ(sut.results()[0].methods[0].name, "method");
    EXPECT_EQ(sut.results()[0].methods[0].returnType, "var");
    EXPECT_TRUE(sut.results()[0].methods[0].parameters.empty());

    EXPECT_EQ(sut.results()[0].methods[1].name, "method2");
    EXPECT_EQ(sut.results()[0].methods[1].returnType, "var");

    ASSERT_EQ(sut.results()[0].methods[1].parameters.size(), 1);
    EXPECT_EQ(sut.results()[0].methods[1].parameters[0].name, "input");
    EXPECT_EQ(sut.results()[0].methods[1].parameters[0].type, "bool");
}

TEST(ClassBuilderTest, Inheritance)
{
    // Arrange
    Cpp::ClassBuilder sut;
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
    EXPECT_EQ(sut.results()[0].type, Cpp::Class::Type::Class);
    ASSERT_EQ(sut.results()[0].parents.size(), 1);
    EXPECT_EQ(sut.results()[0].parents[0], "Class01");
}

TEST(ClassBuilderTest, Composition)
{
    // Arrange
    Cpp::ClassBuilder sut;
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
    EXPECT_EQ(sut.results()[0].type, Cpp::Class::Type::Class);
    ASSERT_EQ(sut.results()[0].variables.size(), 1);
    EXPECT_EQ(sut.results()[0].variables[0].name, "");
    EXPECT_EQ(sut.results()[0].variables[0].type, "Class04");
    EXPECT_EQ(sut.results()[0].variables[0].source, Cpp::Relationship::Composition);
}

TEST(ClassBuilderTest, Aggregation)
{
    // Arrange
    Cpp::ClassBuilder sut;
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
    EXPECT_EQ(sut.results()[0].type, Cpp::Class::Type::Class);
    ASSERT_EQ(sut.results()[0].variables.size(), 1);
    EXPECT_EQ(sut.results()[0].variables[0].name, "");
    EXPECT_EQ(sut.results()[0].variables[0].type, "Class06");
    EXPECT_EQ(sut.results()[0].variables[0].source, Cpp::Relationship::Aggregation);
}

TEST(ClassBuilderTest, LabelsOnRelations)
{
    // Arrange
    Cpp::ClassBuilder sut;
    PlantUml::Parser parser;

    static constexpr auto puml =
        R"(@startuml
        class Class01
        class Class02
        class Class03
        class Class04

        Class01 "1" *-- "many" Class02 : contains
        Class03 o-- Class04 : aggregation
        @enduml)";

    // Act
    act(parser, sut, puml);

    // Assert
    ASSERT_EQ(sut.results().size(), 4);

    EXPECT_EQ(sut.results()[0].name, "Class01");
    EXPECT_EQ(sut.results()[0].type, Cpp::Class::Type::Class);
    ASSERT_EQ(sut.results()[0].variables.size(), 1);
    EXPECT_EQ(sut.results()[0].variables[0].name, "contains");
    EXPECT_EQ(sut.results()[0].variables[0].type, "Class02");
    EXPECT_EQ(sut.results()[0].variables[0].source, Cpp::Relationship::Composition);
    EXPECT_EQ(sut.results()[0].variables[0].cardinality, "many");

    EXPECT_EQ(sut.results()[2].name, "Class03");
    EXPECT_EQ(sut.results()[2].type, Cpp::Class::Type::Class);
    ASSERT_EQ(sut.results()[2].variables.size(), 1);
    EXPECT_EQ(sut.results()[2].variables[0].name, "aggregation");
    EXPECT_EQ(sut.results()[2].variables[0].type, "Class04");
    EXPECT_EQ(sut.results()[2].variables[0].source, Cpp::Relationship::Aggregation);
    EXPECT_EQ(sut.results()[2].variables[0].cardinality, "");
}

TEST(ClassBuilderTest, ExternalMethodsAndVariables)
{
    // Arrange
    Cpp::ClassBuilder sut;
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
    EXPECT_EQ(sut.results()[0].type, Cpp::Class::Type::Class);
    ASSERT_EQ(sut.results()[0].methods.size(), 1);
    EXPECT_EQ(sut.results()[0].methods[0].name, "equals");
    EXPECT_EQ(sut.results()[0].methods[0].returnType, "void");
    EXPECT_TRUE(sut.results()[0].methods[0].parameters.empty());

    EXPECT_EQ(sut.results()[1].name, "ArrayList");
    EXPECT_EQ(sut.results()[1].type, Cpp::Class::Type::Class);
    ASSERT_EQ(sut.results()[1].methods.size(), 1);
    EXPECT_EQ(sut.results()[1].methods[0].name, "size");
    EXPECT_EQ(sut.results()[1].methods[0].returnType, "int");
    EXPECT_TRUE(sut.results()[1].methods[0].parameters.empty());

    ASSERT_EQ(sut.results()[1].variables.size(), 1);
    EXPECT_EQ(sut.results()[1].variables[0].name, "elementData");
    EXPECT_EQ(sut.results()[1].variables[0].type, "Object[]");
    EXPECT_EQ(sut.results()[1].variables[0].source, Cpp::Relationship::Member);
    EXPECT_EQ(sut.results()[1].variables[0].cardinality, "");
}

TEST(ClassBuilderTest, Namespaces)
{
    // Arrange
    Cpp::ClassBuilder sut;
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
    EXPECT_EQ(sut.results()[0].type, Cpp::Class::Type::Class);
    EXPECT_TRUE(sut.results()[0].namespaceStack.empty());

    Cpp::Class person = sut.results()[1];
    EXPECT_EQ(person.name, "Person");
    EXPECT_EQ(person.type, Cpp::Class::Type::Class);
    ASSERT_EQ(person.namespaceStack.size(), 2);
    EXPECT_EQ(person.namespaceStack.back(), "dummy");
    person.namespaceStack.pop_back();
    EXPECT_EQ(person.namespaceStack.back(), "net");
    ASSERT_EQ(person.parents.size(), 1);
    EXPECT_EQ(person.parents[0], "BaseClass");

    EXPECT_EQ(sut.results()[2].name, "BaseClass2");
    EXPECT_EQ(sut.results()[2].type, Cpp::Class::Type::Class);
    EXPECT_TRUE(sut.results()[2].namespaceStack.empty());
}

TEST(ClassBuilderTest, NamespaceSeperator)
{
    // Arrange
    Cpp::ClassBuilder sut;
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
    EXPECT_EQ(foo.type, Cpp::Class::Type::Class);
    ASSERT_EQ(foo.namespaceStack.size(), 2);
    EXPECT_EQ(foo.namespaceStack.back(), "X2");
    foo.namespaceStack.pop_back();
    EXPECT_EQ(foo.namespaceStack.back(), "X1");
}

TEST(ClassBuilderTest, Stereotypes)
{
    // Arrange
    Cpp::ClassBuilder sut;
    PlantUml::Parser parser;

    static constexpr auto puml =
        R"(@startuml

        class System << (S,#FF7700) Singleton >>
        class Date << (D,orchid) >>

        entity test << (T,white) Template >>
        {
            test : var
        }

        @enduml)";

    // Act
    act(parser, sut, puml);

    // Assert
    ASSERT_EQ(sut.results().size(), 3);

    Cpp::Class system = sut.results()[0];
    EXPECT_EQ(system.name, "System");
    EXPECT_EQ(system.stereotype, "Singleton");
    EXPECT_EQ(system.type, Cpp::Class::Type::Class);

    Cpp::Class date = sut.results()[1];
    EXPECT_EQ(date.name, "Date");
    EXPECT_EQ(date.stereotype, "");
    EXPECT_EQ(date.type, Cpp::Class::Type::Class);

    Cpp::Class test = sut.results()[2];
    EXPECT_EQ(test.name, "test");
    EXPECT_EQ(test.stereotype, "Template");
    EXPECT_EQ(test.type, Cpp::Class::Type::Struct);
}
