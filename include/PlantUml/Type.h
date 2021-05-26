#pragma once

#include <list>
#include <string>
#include <vector>

#include "Type.h"

namespace PlantUml {

class Type
{
    std::list<std::string> m_base;

    std::vector<Type> m_templateParams;

};
} // namespace PlantUml
