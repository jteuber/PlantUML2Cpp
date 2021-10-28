#pragma once

#include <memory>
#include <string>

#include "Config.h"
#include "Cpp/Common/CodeGeneratorUtils.h"
#include "Enum.h"

namespace Cpp::Enum {
class HeaderGenerator
{
public:
    explicit HeaderGenerator(std::shared_ptr<Config> config);

    std::string generate(const Enum& in);

private:
    std::shared_ptr<Config> m_config;
    Common::CodeGeneratorUtils m_genUtils;
};
} // namespace Cpp::Enum
