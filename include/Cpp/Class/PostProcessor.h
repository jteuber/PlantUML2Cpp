#pragma once

#include <memory>
#include <set>

#include "Cpp/Class/Class.h"
#include "Cpp/Class/IncludeGatherer.h"
#include "Cpp/Class/MemberSorter.h"

namespace Cpp {
namespace Class {

class PostProcessor
{
public:
    PostProcessor(std::shared_ptr<Config> config);
    void process(std::vector<Class>& classes);

private:
    std::shared_ptr<Config> m_config;

    IncludeGatherer m_gatherer;
    // IncludeOptimizer m_optimizer;
    MemberSorter m_sorter;
};

} // namespace Class
} // namespace Cpp
