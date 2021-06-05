#pragma once

#include <AbstractVisitor.h>
#include <vector>

#include "Class.h"
#include "Container.h"
#include "Element.h"
#include "End.h"
#include "Keyword.h"
#include "Method.h"
#include "Note.h"
#include "Parameter.h"
#include "Relationship.h"
#include "Separator.h"
#include "Variable.h"

namespace Cpp {
namespace Variant {

class Translator : public PlantUml::AbstractVisitor
{
public:
    bool visit(Variable v);
    bool visit(Method m);
    bool visit(Relationship r);
    bool visit(Container c);
    bool visit(Element e);
    bool visit(Note n);
    bool visit(Separator s);
    bool visit(Keyword k);
    bool visit(Parameter p);
    bool visit(End e);
    std::vector<Class> results();
};
} // namespace Variant
} // namespace Cpp
