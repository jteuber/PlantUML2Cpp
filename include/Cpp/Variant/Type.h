#pragma once

#include <list>
#include <string>
#include <vector>

#include "Type.h"

namespace Cpp {
namespace Variant {

struct Type
{
private:
    std::list<std::string> base;
    std::vector<Type> templateParams;
};
} // namespace Variant
} // namespace Cpp
