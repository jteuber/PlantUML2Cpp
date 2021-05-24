#include "PlantUml/SyntaxNode.h"

namespace PlantUml {

void SyntaxNode::visit(AbstractVisitor& visitor)
{
    bool visitChildren = std::visit([&visitor](auto&& arg) -> bool { return visitor.visit(arg); }, element);
    if (visitChildren) {
        for (auto& child : children) {
            child.visit(visitor);
        }
    }
}

} // namespace PlantUml
