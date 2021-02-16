#pragma once

#include <stack>
#include <string_view>
#include <vector>

enum class Visibility
{
    Private,
    Protected,
    PackagePrivate,
    Public,
    Unspecified
};

enum class Relationship
{
    Extension,
    Composition,
    Aggregation,
    Usage,
    Member
};

struct Variable
{
    std::string_view name;
    std::string_view type;
    std::string_view cardinality;
    Visibility visibility = Visibility::Unspecified;
    Relationship source;
};

struct Parameter
{
    std::string_view name;
    std::string_view type;
};

struct Method
{
    std::string_view name;
    std::string_view returnType;
    std::vector<Parameter> parameters;
    Visibility visibility = Visibility::Unspecified;
};

struct Class
{
    std::string_view name;
    std::vector<std::string_view> parents;
    std::stack<std::string_view> namespaceStack;
    std::vector<Variable> variables;
    std::vector<Method> methods;

    enum class Type
    {
        Abstract,
        Class,
        Interface,
        Struct
    } type = Type::Class;
};
