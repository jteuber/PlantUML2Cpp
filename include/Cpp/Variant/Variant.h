#pragma once

#include <list>
#include <string>

#include "Cpp/Type.h"

namespace Cpp {
namespace Variant {

struct Variant
{
    std::string name;
    std::list<std::string> namespaces;
    std::string comment;
    Type containedTypes;
};
} // namespace Variant
} // namespace Cpp
