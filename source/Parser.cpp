#include "Parser.h"

#include <iostream>

Parser::Parser(/* args */)
{
    // ========= GENERAL KEYWORDS =========
    g.setSeparator(g["Whitespace"] << "[\t ]");
    g["Identifier"] << "[a-zA-Z] [a-zA-Z0-9_]*";
    g.setProgramRule("QuotedName", peg_parser::presets::createStringProgram("\"", "\""),
                   [](auto e, auto& p) {});
    g["Name"] << "Identifier | QuotedName";

    g["Colon"] << "':'";
    g["Endl"]  << "'\n'";

    // ========= ELEMENTS =========
    // keywords
    g["KW_Class"]    << "'class'";
    g["KW_Abstract"] << "'abstract' KW_Class?";
    g["KW_Circle"]   << "'circle' | '()'";
    g["KW_Diamond"]  << "'diamond' | '<>'";

    // simple elements
    g["Abstract"]   << "KW_Abstract Name"  >> [](auto e, auto& p) { p.elements.push_back(PlantUML::Element{PlantUML::Element::Type::Abstract,   e[1].string()}); };
    g["Annotation"] << "'annotation' Name" >> [](auto e, auto& p) { p.elements.push_back(PlantUML::Element{PlantUML::Element::Type::Annotation, e[0].string()}); };
    g["Circle"]     << "KW_Circle Name"    >> [](auto e, auto& p) { p.elements.push_back(PlantUML::Element{PlantUML::Element::Type::Circle,     e[1].string()}); };
    g["Class"]      << "'class' Name"      >> [](auto e, auto& p) { p.elements.push_back(PlantUML::Element{PlantUML::Element::Type::Class,      e[0].string()}); };
    g["Diamond"]    << "KW_Diamond Name"   >> [](auto e, auto& p) { p.elements.push_back(PlantUML::Element{PlantUML::Element::Type::Diamond,    e[1].string()}); };
    g["Entity"]     << "'entity' Name"     >> [](auto e, auto& p) { p.elements.push_back(PlantUML::Element{PlantUML::Element::Type::Entity,     e[0].string()}); };
    g["Enum"]       << "'enum' Name"       >> [](auto e, auto& p) { p.elements.push_back(PlantUML::Element{PlantUML::Element::Type::Enum,       e[0].string()}); };
    g["Interface"]  << "'interface' Name"  >> [](auto e, auto& p) { p.elements.push_back(PlantUML::Element{PlantUML::Element::Type::Interface,  e[0].string()}); };

    // variables, parameters and methods
    g["VariableDef"] << "(Identifier Identifier | '{field}' Identifier+ | Identifier ':' Identifier) Endl";
    g["ParamList"]   << "'(' (VariableDef (',' VariableDef)*) ')'";
    g["MethodDef"]   << "(Identifier Identifier ParamList | '{method}' Identifier+ | Identifier ParamList ':' Identifier) Endl";

    // elements with bodies
    g["OpenBrackets"]     << "Endl? '{' Endl?";
    g["CloseBrackets"]     << "'}'";
    g["ClassWithBody"]     << "Class OpenBrackets (VariableDef | MethodDef)* CloseBrackets";
    g["EntityWithBody"]    << "Entity OpenBrackets VariableDef* CloseBrackets";
    g["InterfaceWithBody"] << "Interface OpenBrackets MethodDef* CloseBrackets";
    
    // collector rule
    g["Element"] << "ClassWithBody | EntityWithBody | InterfaceWithBody | Abstract | Annotation | Circle | Class | Diamond | Entity | Enum | Interface";


    // ========= RELATIONSHIPS =========
    g["TriangleLeft"]  << "'<|'";
    g["TriangleRight"] << "'|>'";
    g["Composition"]   << "'*'";
    g["Aggregation"]   << "'o'";

    g["RelationshipLabel"] << "Colon '<'? Name* '>'?";

    g["ExtensionLeft"]    << "Identifier QuotedName? TriangleLeft '-'+ QuotedName? Identifier RelationshipLabel?";
    g["CompositionLeft"]  << "Identifier QuotedName? Composition '-'+ QuotedName? Identifier RelationshipLabel?";
    g["AggregationLeft"]  << "Identifier QuotedName? Aggregation '-'+ QuotedName? Identifier RelationshipLabel?";
    g["RelationshipLeft"] << "ExtensionLeft | CompositionLeft | AggregationLeft";

    g["ExtensionRight"]    << "Identifier QuotedName? '-'+ TriangleRight QuotedName? Identifier RelationshipLabel?";
    g["CompositionRight"]  << "Identifier QuotedName? '-'+ Composition QuotedName? Identifier RelationshipLabel?";
    g["AggregationRight"]  << "Identifier QuotedName? '-'+ Aggregation QuotedName? Identifier RelationshipLabel?";
    g["RelationshipRight"] << "ExtensionRight | CompositionRight | AggregationRight";

    g["Relationship"] << "RelationshipLeft | RelationshipRight";

    // ========= DIAGRAM =========
    g["Body"] << "((Element | Relationship)? Endl)*";
    g["Start"] << "'@startuml' Endl";
    g["End"] << "'@enduml' Endl?";
    g["Diagram"] << "Start Body End";
    
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