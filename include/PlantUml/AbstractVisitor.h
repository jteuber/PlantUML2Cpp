#pragma once

#include "ModelElement.h"
#include <variant>

namespace PlantUml {

class AbstractVisitor
{
public:
    virtual bool visit(const Variable& v)     = 0;
    virtual bool visit(const Method& m)       = 0;
    virtual bool visit(const Relationship& r) = 0;
    virtual bool visit(const Container& c)    = 0;
    virtual bool visit(const Element& e)      = 0;
    virtual bool visit(const Note& n)         = 0;
    virtual bool visit(const Separator& s)    = 0;
    virtual bool visit(const Enumerator& e)   = 0;
    virtual bool visit(const Parameter& p)    = 0;
    virtual bool visit(const End& e)          = 0;

    virtual bool visit(const std::string& /*s*/) final
    {
        return false;
    };
    virtual bool visit(const ContainerType& /*ct*/) final
    {
        return false;
    };
    virtual bool visit(const ElementType& /*ct*/) final
    {
        return false;
    };
    virtual bool visit(const RelationshipType& /*ct*/) final
    {
        return false;
    };
    virtual bool visit(const Visibility& /*ct*/) final
    {
        return false;
    };
    virtual bool visit(const Modifier& /*ct*/) final
    {
        return false;
    };
};

} // namespace PlantUml