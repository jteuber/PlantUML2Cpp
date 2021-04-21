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
    Extension,
    Composition,
    Aggregation,
    Usage
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

    auto operator<=>(const Container&) const = default;
};

struct Element
{
    std::list<std::string> name;
    std::string stereotype;
    char spotLetter;
    std::list<std::string> implements;
    std::list<std::string> extends;
    ElementType type;

    auto operator<=>(const Element&) const = default;
};

struct Enumerator
{
    std::string name;

    auto operator<=>(const Enumerator&) const = default;
};

struct Parameter
{
    std::string name;
    std::list<std::string> type;
    bool isConst;

    auto operator<=>(const Parameter&) const = default;
};

struct Method
{
    std::string name;
    std::list<std::string> returnType;
    std::list<std::string> element;
    Visibility visibility;
    bool isAbstract;
    bool isConst;
    bool isStatic;

    auto operator<=>(const Method&) const = default;
};

struct Note
{
    std::string name;
    std::list<std::string> relatesTo;
    std::string text;

    auto operator<=>(const Note&) const = default;
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

    auto operator<=>(const Relationship&) const = default;
};

struct Separator
{
    std::string text;

    auto operator<=>(const Separator&) const = default;
};

struct Variable
{
    std::string name;
    std::list<std::string> type;
    std::list<std::string> element;
    Visibility visibility;
    bool isConst;
    bool isStatic;

    auto operator<=>(const Variable&) const = default;
};

struct End
{
    EndType type;

    auto operator<=>(const End&) const = default;
};

using ModelElement = std::variant<std::string,
                                  Note,
                                  Separator,
                                  Enumerator,
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

struct SyntaxNode
{
    ModelElement element;
    std::vector<SyntaxNode> children;
};

} // namespace PlantUml
