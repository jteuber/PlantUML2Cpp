#pragma once

#include <list>
#include <string>
#include <vector>

#include "Cpp/Common/Type.h"

namespace Cpp::Variant {

struct Variant
{
    std::string name;
    std::list<std::string> namespaces;
    std::string comment;
    std::vector<Common::Type> containedTypes;
};
} // namespace Cpp::Variant
