#pragma once

#include <memory>
#include <set>

#include "Cpp/Class.h"
#include "Cpp/IncludeGatherer.h"
#include "Cpp/IncludeOptimizer.h"

namespace Cpp {

class ClassPostProcessor
{
public:
    void process(std::vector<Class>& classes);

private:
    IncludeGatherer gatherer;
    IncludeOptimizer optimizer;
};

} // namespace Cpp
