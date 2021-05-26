#pragma once

#include <list>
#include <string>

#include "RelationshipType.h"

namespace PlantUml {

class Relationship
{
    std::list<std::string> m_subject;

    std::list<std::string> m_object;

    std::string m_subjectCardinality;

    std::string m_objectCardinality;

    std::string m_label;

    bool m_hidden;

    RelationshipType m_type;

};
} // namespace PlantUml
