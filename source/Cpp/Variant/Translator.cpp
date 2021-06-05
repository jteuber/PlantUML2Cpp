#include "Cpp/Variant/Translator.h"

namespace Cpp {
namespace Variant {

bool Translator::visit(const PlantUml::Variable& v) {}

bool Translator::visit(const PlantUml::Method& m) {}

bool Translator::visit(const PlantUml::Relationship& r) {}

bool Translator::visit(const PlantUml::Container& c) {}

bool Translator::visit(const PlantUml::Element& e) {}

bool Translator::visit(const PlantUml::Note& n) {}

bool Translator::visit(const PlantUml::Separator& s) {}

bool Translator::visit(const PlantUml::Enumerator& k) {}

bool Translator::visit(const PlantUml::Parameter& p) {}

bool Translator::visit(const PlantUml::End& e) {}

bool Translator::visit(const PlantUml::Type& /*t*/)
{
    return false;
}

std::vector<Variant> Translator::results() {}

} // namespace Variant
} // namespace Cpp
