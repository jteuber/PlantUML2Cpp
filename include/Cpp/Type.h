#pragma once

#include <list>
#include <string>
#include <vector>

#include "Type.h"

namespace Cpp {

struct Type
{
    std::string base;
    std::vector<Type> templateParams;

    auto operator<=>(const Type&) const = default;
};
} // namespace Cpp
