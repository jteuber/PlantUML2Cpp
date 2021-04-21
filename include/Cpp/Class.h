#pragma once

#include <list>
#include <string>
#include <variant>
#include <vector>

namespace Cpp {

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
    std::string comment;
    bool isConst  = false;
    bool isStatic = false;
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
    std::string comment;
    bool isAbstract = false;
    bool isConst    = false;
    bool isStatic   = false;
    std::vector<Parameter> parameters;
};

struct VisibilityKeyword
{
    std::string name;
};

struct Separator
{
    std::string text;
};

using BodyElement = std::variant<std::string, Variable, Method, VisibilityKeyword, Separator>;

struct Class
{
    std::string name;
    std::string comment;
    bool isInterface = false;
    bool isStruct    = false;
    std::list<std::string> namespaces;
    std::vector<std::string> inherits;
    std::vector<std::string> includes;
    std::vector<BodyElement> body;
};

} // namespace Cpp