#include "Cpp/MemberSorter.h"

namespace Cpp {

void MemberSorter::sort(Class& c)
{
    enum class Visibility
    {
        Public,
        Private,
        Protected
    } lastVis = Visibility::Private;

    if (c.isStruct) {
        lastVis = Visibility::Public;
    }

    for (auto it = c.body.begin(); it != c.body.end(); ++it) {
        if (std::holds_alternative<VisibilityKeyword>(*it)) {
            break;
        } else if (std::holds_alternative<Variable>(*it) && lastVis != Visibility::Private) {
            it      = c.body.insert(it, VisibilityKeyword{"private:"});
            lastVis = Visibility::Private;
        } else if (std::holds_alternative<Method>(*it) && lastVis != Visibility::Public) {
            it      = c.body.insert(it, VisibilityKeyword{"public:"});
            lastVis = Visibility::Public;
        }
    }
}

} // namespace Cpp
