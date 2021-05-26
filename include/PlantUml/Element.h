#pragma once

#include <list>
#include <string>

#include "ElementType.h"
#include "char.h"

namespace PlantUml {

class Element
{
    std::list<std::string> m_name;

    std::string m_stereotype;

    char m_spotLetter;

    std::list<std::string> m_implements;

    std::list<std::string> m_extends;

    ElementType m_type;

};
} // namespace PlantUml
