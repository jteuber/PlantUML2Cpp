#ifndef SYNTAXNODE_H
#define SYNTAXNODE_H

#include <vector>

#include "PlantUml/AbstractVisitor.h"
#include "PlantUml/ModelElement.h"

namespace PlantUml {

class SyntaxNode
{
public:
    void visit(AbstractVisitor& visitor) const;

    ModelElement element;
    std::vector<SyntaxNode> children;
};

} // namespace PlantUml

#endif // SYNTAXNODE_H
