#pragma once

#include <list>
#include <string>
#include <variant>
#include <vector>

namespace PlantUml {

enum class ContainerType
{
    Document,
    Package,
    Namespace
};
enum class ElementType
{
    Abstract,
    Annotation,
    Class,
    Entity,
    Enum,
    Interface
};
enum class RelationshipType
{
    Extension,      // --|>
    Composition,    // --*
    Aggregation,    // --o
    Usage,          // -->
    Implementation, // -- towards ()
    Requirement     // --(
};
enum class Visibility
{
    Private,
    Protected,
    PackagePrivate,
    Public,
    Unspecified
};
enum class EndType
{
    Document,
    Package,
    Namespace,
    Element,
    Method
};

struct Container
{
    std::list<std::string> name;
    std::string style;
    ContainerType type;

    bool operator==(const Container&) const = default;
};

struct Element
{
    std::list<std::string> name;
    std::string stereotype;
    char spotLetter;
    std::list<std::string> implements;
    std::list<std::string> extends;
    ElementType type;

    bool operator==(const Element&) const = default;
};

struct Enumerator
{
    std::string name;

    bool operator==(const Enumerator&) const = default;
};

struct Type
{
    std::list<std::string> base;
    std::vector<Type> templateParams;

    bool operator==(const Type&) const = default;
};

struct Parameter
{
    std::string name;
    Type type;
    bool isConst;

    bool operator==(const Parameter&) const = default;
};

struct Method
{
    std::string name;
    Type returnType;
    std::list<std::string> element;
    Visibility visibility;
    bool isAbstract;
    bool isConst;
    bool isStatic;

    bool operator==(const Method&) const = default;
};

struct Note
{
    std::string name;
    std::list<std::string> relatesTo;
    std::string text;

    bool operator==(const Note&) const = default;
};

struct Relationship
{
    std::list<std::string> subject;
    std::list<std::string> object;
    std::string subjectCardinality;
    std::string objectCardinality;
    std::string label;
    bool hidden;
    RelationshipType type;

    bool operator==(const Relationship&) const = default;
};

struct Separator
{
    std::string text;

    bool operator==(const Separator&) const = default;
};

struct Variable
{
    std::string name;
    Type type;
    std::list<std::string> element;
    Visibility visibility;
    bool isConst;
    bool isStatic;

    bool operator==(const Variable&) const = default;
};

struct End
{
    EndType type;

    bool operator==(const End&) const = default;
};

using ModelElement = std::variant<std::string,
                                  Note,
                                  Separator,
                                  Enumerator,
                                  Type,
                                  Parameter,
                                  Container,
                                  Element,
                                  Relationship,
                                  Variable,
                                  Method,
                                  End,
                                  ContainerType,
                                  ElementType,
                                  RelationshipType,
                                  Visibility>;

} // namespace PlantUml
