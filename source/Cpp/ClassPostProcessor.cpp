#include "Cpp/ClassPostProcessor.h"

Cpp::ClassPostProcessor::ClassPostProcessor(std::shared_ptr<Config> config)
    : m_config(config)
    , m_gatherer(m_config)
{}

void Cpp::ClassPostProcessor::process(std::vector<Cpp::Class>& classes)
{
    for (auto& c : classes) {
        m_gatherer.gather(c);
        m_sorter.sort(c);
    }
}
