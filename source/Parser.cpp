#include "Parser.h"

Parser::Parser(/* args */)
{
    g.setSeparator(g["Whitespace"] << "[\t \n]");
    g["Identifier"] << "[a-zA-Z] [a-zA-Z0-9_]*";

    // element keywords
    g["KW_Abstract"] << "'abstract' 'class'?";
    g["KW_Circle"] << "'circle' | '()'";
    g["KW_Diamond"] << "'diamond' | '<>'";

    // elements
    g["Abstract"] << "KW_Abstract Identifier" >> [](auto e, auto& p) { p.elements.push_back(PlantUML::Element{PlantUML::Element::Type::Abstract, e[1].string()}); };
    g["Annotation"] << "'annotation' Identifier" >> [](auto e, auto& p) { p.elements.push_back(PlantUML::Element{PlantUML::Element::Type::Annotation, e[0].string()}); };
    g["Circle"] << "KW_Circle Identifier" >> [](auto e, auto& p) { p.elements.push_back(PlantUML::Element{PlantUML::Element::Type::Circle, e[1].string()}); };
    g["Class"] << "'class' Identifier" >> [](auto e, auto& p) { p.elements.push_back(PlantUML::Element{PlantUML::Element::Type::Class, e[0].string()}); };
    g["Diamond"] << "KW_Diamond Identifier" >> [](auto e, auto& p) { p.elements.push_back(PlantUML::Element{PlantUML::Element::Type::Diamond, e[1].string()}); };
    g["Entity"] << "'entity' Identifier" >> [](auto e, auto& p) { p.elements.push_back(PlantUML::Element{PlantUML::Element::Type::Entity, e[0].string()}); };
    g["Enum"] << "'enum' Identifier" >> [](auto e, auto& p) { p.elements.push_back(PlantUML::Element{PlantUML::Element::Type::Enum, e[0].string()}); };
    g["Interface"] << "'interface' Identifier" >> [](auto e, auto& p) { p.elements.push_back(PlantUML::Element{PlantUML::Element::Type::Interface, e[0].string()}); };
    g["Element"] << "Abstract | Annotation | Circle | Class | Diamond | Entity | Enum | Interface";

    g["VariableDef"] << "Identifier Identifier | '{field}' Identifier+ | Identifier ':' Identifier";

    g["ParamList"] << "'(' (VariableDef (',' VariableDef)*) ')'";

    g["MethodDef"] << "Identifier Identifier ParamList | '{method}' Identifier+ | Identifier ParamList ':' Identifier";

    g["ClassWithBody"] << "Class '{' (VariableDef | MethodDef)* '}'";
    g["EntityWithBody"] << "Entity '{' VariableDef* '}'";

    g["ExtensionLeft"] << "Identifier '<|-' '-'? Identifier";
    g["ExtensionRight"] << "Identifier '-'? '-|>' Identifier";

    g["CompositionLeft"] << "Identifier '*-' '-'? Identifier";
    g["CompositionRight"] << "Identifier '-'? '-*' Identifier";

    g["AggregationLeft"] << "Identifier 'o-' '-'? Identifier";
    g["AggregationRight"] << "Identifier '-'? '-o' Identifier";

    // diagram
    g["Start"] << "'@startuml'";
    g["End"] << "'@enduml'";
    g["Diagram"] << "Start Element* End";
    
    g.setStart(g["Diagram"]);
}

Parser::~Parser()
{
}

PlantUML Parser::parse(const std::string& input) 
{
    PlantUML result;
    g.run(input, result);

    return result;
}