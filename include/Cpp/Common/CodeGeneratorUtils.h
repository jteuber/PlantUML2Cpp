#pragma once

#include <list>
#include <memory>
#include <string>

#include "Config.h"

#include "PlantUml/ModelElement.h"

namespace Cpp::Common {

class CodeGeneratorUtils
{
public:
    explicit CodeGeneratorUtils(std::shared_ptr<Config> config);

    std::string openNamespaces(const std::list<std::string>& namespaces);
    std::string closeNamespaces(const std::list<std::string>& namespaces);

private:
    std::shared_ptr<Config> m_config;
};

} // namespace Cpp::Common