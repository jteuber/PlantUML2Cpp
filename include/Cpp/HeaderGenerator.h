#pragma once

#include <memory>
#include <set>

#include "Cpp/Class.h"

namespace Cpp {

class HeaderGenerator
{
public:
    explicit HeaderGenerator();

    std::string generate(const Class& in);

    // private:
    //    std::string generateMethods(const std::vector<Method>& methods, Visibility vis);
    //    std::string generateMembers(const std::vector<Variable>& members, Visibility vis, Class::Type classType);
};

} // namespace Cpp
