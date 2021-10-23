#include "Cpp/Enum/Translator.h"

namespace Cpp::Enum {
bool Translator::visit(const PlantUml::Variable& v) {}

bool Translator::visit(const PlantUml::Method& m) {}

bool Translator::visit(const PlantUml::Relationship& r) {}

bool Translator::visit(const PlantUml::Container& c) {}

bool Translator::visit(const PlantUml::Element& e) {}

bool Translator::visit(const PlantUml::Note& n) {}

bool Translator::visit(const PlantUml::Separator& s) {}

bool Translator::visit(const PlantUml::Enumerator& e) {}

bool Translator::visit(const PlantUml::Parameter& p) {}

bool Translator::visit(const PlantUml::End& e) {}

bool Translator::visit(const PlantUml::Type& t) {}

std::vector<Enum> Translator::results() && {}

} // namespace Cpp::Enum
