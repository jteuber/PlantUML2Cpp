#pragma once

#include <list>
#include <string>

#include "Modifier.h"
#include "Type.h"
#include "Visibility.h"

namespace PlantUml {

class Variable
{
    std::string m_name;

    Type m_type;

    std::list<std::string> m_element;

    Visibility m_visibility;

    Modifier m_modifier;

    Type m_type;

};
} // namespace PlantUml
