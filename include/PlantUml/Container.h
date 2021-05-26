#pragma once

#include <list>
#include <string>

#include "ContainerType.h"

namespace PlantUml {

class Container
{
    std::list<std::string> m_name;

    std::string m_style;

    ContainerType m_type;

};
} // namespace PlantUml
