#pragma once

#include <list>
#include <string>
#include <vector>

#include "Enumerator.h"

namespace Cpp::Enum {
struct Enum
{
    std::string name;
    std::list<std::string> namespaces;
    std::string comment;
    std::vector<Enumerator> enumerators;
};
} // namespace Cpp::Enum
