#include "Cpp/Class/PostProcessor.h"

namespace Cpp {
namespace Class {

PostProcessor::PostProcessor(std::shared_ptr<Config> config)
    : m_config(config)
    , m_gatherer(m_config)
{}

void PostProcessor::process(std::vector<Class>& classes)
{
    for (auto& c : classes) {
        m_gatherer.gather(c);
        m_sorter.sort(c);
    }
}

} // namespace Class
} // namespace Cpp
