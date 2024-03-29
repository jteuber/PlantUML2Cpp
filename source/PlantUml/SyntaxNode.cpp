#include "PlantUml/SyntaxNode.h"

namespace PlantUml {

void SyntaxNode::visit(AbstractVisitor& visitor) const
{
    bool visitChildren = std::visit([&visitor](auto&& arg) -> bool { return visitor.visit(arg); }, element);
    if (visitChildren) {
        for (const auto& child : children) {
            child.visit(visitor);
        }
    }
}

} // namespace PlantUml
