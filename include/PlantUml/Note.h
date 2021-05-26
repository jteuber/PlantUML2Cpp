#pragma once

#include <list>
#include <string>

namespace PlantUml {

class Note
{
    std::string m_name;

    std::list<std::string> m_relatesTo;

    std::string m_text;

};
} // namespace PlantUml
