#pragma once

#include <string>

#include "Class.h"

namespace Cpp::Class {

class SourceGenerator
{
public:
    std::string generate(const Class& in);

private:
    std::string typeToString(const Common::Type& t);
};

} // namespace Cpp::Class
