#pragma once

#include <string>

#include "Variant.h"

namespace Cpp::Variant {

class HeaderGenerator
{
public:
    std::string generate(const Variant& in);
};
} // namespace Cpp::Variant
