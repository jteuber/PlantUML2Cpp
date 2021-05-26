#pragma once

#include <list>
#include <string>
#include <vector>

#include "Modifier.h"
#include "Parameter.h"
#include "Type.h"
#include "Visibility.h"

namespace PlantUml {

class Method
{
    std::string m_name;

    Type m_returnType;

    std::list<std::string> m_element;

    Visibility m_visibility;

    Modifier m_modifier;

    Type m_returnType;

    std::vector<Parameter> m_parameters;

};
} // namespace PlantUml
