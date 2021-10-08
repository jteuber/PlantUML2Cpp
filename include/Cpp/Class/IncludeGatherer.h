#pragma once

#include <memory>
#include <set>

#include "Config.h"

#include "Cpp/Class/Class.h"

namespace Cpp::Class {

class IncludeGatherer
{
public:
    explicit IncludeGatherer(std::shared_ptr<Config> config);
    void gather(Class& c);

private:
    // helper methods
    std::set<std::string> decomposeType(const Common::Type& type);

    std::shared_ptr<Config> m_config;
};

} // namespace Cpp::Class
