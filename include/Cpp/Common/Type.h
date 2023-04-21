#pragma once

#include <list>
#include <string>
#include <vector>

namespace Cpp::Common {

struct Type
{
    std::string base;
    std::vector<Type> templateParams;

    bool operator==(const Type&) const = default;
};
} // namespace Cpp::Common
