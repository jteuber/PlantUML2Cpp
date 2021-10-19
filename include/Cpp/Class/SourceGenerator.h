#pragma once

#include <string>

#include "Class.h"
#include "Cpp/Common/CodeGeneratorUtils.h"

namespace Cpp::Class {

class SourceGenerator
{
public:
    explicit SourceGenerator(std::shared_ptr<Config> config);
    std::string generate(const Class& in);

private:
    std::string typeToString(const Common::Type& t);

    std::shared_ptr<Config> m_config;
    Common::CodeGeneratorUtils m_genUtils;
};

} // namespace Cpp::Class
