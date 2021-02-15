#pragma once

#include <string_view>
#include <vector>
#include <stack>

enum class Visibility {
    Private,
    Protected,
    PackagePrivate,
    Public
};

struct Variable
{
    std::string_view name;
    std::string_view type;
    Visibility visibility;
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
    Visibility visibility;
};

struct Class
{
    std::string_view name;
    std::stack<std::string_view> namespaceStack;
    std::vector<Variable> variables;
    std::vector<Method> methods;

    enum class Type {
        Abstract,
        Class,
        Interface,
        Struct
    } type;
};