#pragma once

#include <list>
#include <string>
#include <vector>

namespace Cpp::Common {

struct Type
{
    std::string base;
    std::vector<Type> templateParams;

    auto operator<=>(const Type&) const = default;
};
} // namespace Cpp::Common
