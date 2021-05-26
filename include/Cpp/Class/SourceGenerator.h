#pragma once

#include <string>

#include "Class.h"

namespace Cpp {
namespace Class {

class SourceGenerator
{
public:
    std::string generate(const Class& in);

private:
    std::string typeToString(const Type& t);
};

} // namespace Class
} // namespace Cpp
