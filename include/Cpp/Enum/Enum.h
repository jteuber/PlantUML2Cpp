#pragma once

#include <list>
#include <string>

#include "Keyword.h"

namespace Cpp::Enum {
struct Enum
{
    std::string name;
    std::list<std::string> namespaces;
    std::string comment;
    Keyword keyword;
};
} // namespace Cpp::Enum
