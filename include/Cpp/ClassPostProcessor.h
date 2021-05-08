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
    ClassPostProcessor(std::shared_ptr<Config> config);
    void process(std::vector<Class>& classes);

private:
    std::shared_ptr<Config> m_config;

    IncludeGatherer m_gatherer;
    // IncludeOptimizer m_optimizer;
};

} // namespace Cpp
