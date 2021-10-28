#include "Cpp/Enum/HeaderGenerator.h"

namespace Cpp::Enum {
HeaderGenerator::HeaderGenerator(std::shared_ptr<Config> config)
    : m_config(std::move(config))
    , m_genUtils(m_config)
{}

std::string HeaderGenerator::generate(const Enum& in) {}

} // namespace Cpp::Enum
