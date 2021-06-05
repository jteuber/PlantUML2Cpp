#include "Cpp/Variant/Translator.h"

namespace Cpp {
namespace Variant {

bool Translator::visit(Variable v) {}

bool Translator::visit(Method m) {}

bool Translator::visit(Relationship r) {}

bool Translator::visit(Container c) {}

bool Translator::visit(Element e) {}

bool Translator::visit(Note n) {}

bool Translator::visit(Separator s) {}

bool Translator::visit(Keyword k) {}

bool Translator::visit(Parameter p) {}

bool Translator::visit(End e) {}

std::vector<Class> Translator::results() {}

} // namespace Variant
} // namespace Cpp
