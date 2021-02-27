#pragma once

#include <stack>
#include <string>
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
    std::string name;
    std::string type;
    std::string cardinality;
    Visibility visibility = Visibility::Unspecified;
    Relationship source;
};

struct Parameter
{
    std::string name;
    std::string type;
};

struct Method
{
    std::string name;
    std::string returnType;
    std::vector<Parameter> parameters;
    Visibility visibility = Visibility::Unspecified;
};

struct Class
{
    std::string name;
    std::string stereotype;
    std::vector<std::string> parents;
    std::stack<std::string> namespaceStack;
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
