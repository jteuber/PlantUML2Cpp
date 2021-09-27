#pragma once

#include <list>
#include <set>
#include <string>
#include <variant>
#include <vector>

#include "Cpp/Type.h"

namespace Cpp {
namespace Class {

struct Variable
{
    std::string name;
    Type type;
    std::string comment;
    bool isConst  = false;
    bool isStatic = false;
};

struct Parameter
{
    std::string name;
    Type type;
};

struct Method
{
    std::string name;
    Type returnType;
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

using ClassElement = std::variant<std::string, Variable, Method, VisibilityKeyword, Separator>;

struct Class
{
    std::string name;
    std::string comment;
    bool isInterface = false;
    bool isStruct    = false;
    std::list<std::string> namespaces;
    std::vector<std::string> inherits;
    std::set<std::string> localIncludes;
    std::set<std::string> externalIncludes;
    std::vector<ClassElement> body;
};

} // namespace Class
} // namespace Cpp
