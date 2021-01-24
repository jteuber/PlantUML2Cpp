#include "Parser.h"

#include <iostream>
#include <algorithm>

Parser::Parser(/* args */)
{
    // ========= GENERAL KEYWORDS =========
    g.setSeparator(g["Whitespace"] << "[\t ]");
    g["Identifier"] << "[a-zA-Z] [a-zA-Z0-9_.]*";
    g.setProgramRule("QuotedName", peg_parser::presets::createStringProgram("\"", "\""),
                   [](auto e, auto& p) {});
    g["Name"] << "Identifier | QuotedName";
    g["FieldTypename"] << "[a-zA-Z] [a-zA-Z0-9_.:<>]* '[]'?";

    g["Colon"] << "':'";
    g["Endl"]  << "'\n'";
    g["Color"]  << "'#' (!(' ' | ')' | Endl) .)* | Identifier";

    g["OpenBrackets"]     << "Endl? '{'";
    g["CloseBrackets"]     << "'}'";

    // ========= COMMENTS =========
    g["Comment"] << "'\\'' (!Endl .)*";
    g["Include"] << "'!include' (!Endl .)*"; // ignore !include

    g["Ignored"] << "Comment | Include";

    // ========= ELEMENTS =========
    // fields, parameters and methods
    g["FieldDef"] << "'{field}' Identifier+ | Identifier Colon FieldTypename | FieldTypename Identifier?";
    g["ParamList"]   << "'(' (FieldDef (',' FieldDef)*)? ')'";
    g["MethodDef"]   << "'{method}' Identifier+ | Identifier ParamList ':' FieldTypename | FieldTypename Identifier? ParamList";

    // external fields and methods
    g["ExtFieldDef"] << "Identifier Colon FieldDef";
    g["ExtMethodDef"] << "Identifier Colon MethodDef";
    g["ExternalDefinitions"] << "ExtMethodDef | ExtFieldDef";

    // ========= CONTAINERS =========
    // keywords
    g["KW_Class"]    << "'class'";
    g["KW_Abstract"] << "'abstract' KW_Class?";
    g["KW_Circle"]   << "'circle' | '()'";
    g["KW_Diamond"]  << "'diamond' | '<>'";

    // stereotypes
    g["Spot"] << "'(' . ',' Color ')'";
    g["Stereotype"] << "'<<' Spot? Identifier? '>>'";

    // simple containers
    g["Abstract"]   << "KW_Abstract Name Stereotype?"  >> [](auto e, auto& p) { p.containers.push_back(Container{Container::Type::Abstract,   e[1].string()}); };
    g["Annotation"] << "'annotation' Name Stereotype?" >> [](auto e, auto& p) { p.containers.push_back(Container{Container::Type::Annotation, e[0].string()}); };
    g["Circle"]     << "KW_Circle Name Stereotype?"    >> [](auto e, auto& p) { p.containers.push_back(Container{Container::Type::Circle,     e[1].string()}); };
    g["Class"]      << "'class' Name Stereotype?"      >> [](auto e, auto& p) { p.containers.push_back(Container{Container::Type::Class,      e[0].string()}); };
    g["Diamond"]    << "KW_Diamond Name Stereotype?"   >> [](auto e, auto& p) { p.containers.push_back(Container{Container::Type::Diamond,    e[1].string()}); };
    g["Entity"]     << "'entity' Name Stereotype?"     >> [](auto e, auto& p) { p.containers.push_back(Container{Container::Type::Entity,     e[0].string()}); };
    g["Enum"]       << "'enum' Name Stereotype?"       >> [](auto e, auto& p) { p.containers.push_back(Container{Container::Type::Enum,       e[0].string()}); };
    g["Interface"]  << "'interface' Name Stereotype?"  >> [](auto e, auto& p) { p.containers.push_back(Container{Container::Type::Interface,  e[0].string()}); };
    // TODO: stereotypes << ... >>

    // containers with bodies
    g["ClassWithBody"]     << "Class OpenBrackets ((MethodDef | FieldDef | Comment)? Endl)* CloseBrackets";
    g["EntityWithBody"]    << "Entity OpenBrackets ((FieldDef | Comment)? Endl)* CloseBrackets";
    g["InterfaceWithBody"] << "Interface OpenBrackets ((MethodDef | Comment)? Endl)* CloseBrackets";
    g["EnumWithBody"]      << "Enum OpenBrackets (Identifier? Endl)* CloseBrackets";
    
    // collector rule
    g["Container"] << "ClassWithBody | EntityWithBody | InterfaceWithBody | EnumWithBody | Abstract | Annotation | Circle | Class | Diamond | Entity | Enum | Interface ";



    // ========= RELATIONSHIPS =========
    g["TriangleLeft"]  << "'<|'";
    g["TriangleRight"] << "'|>'";
    g["Composition"]   << "'*'";
    g["Aggregation"]   << "'o'";
    g["OpenTriLeft"]   << "'<'";
    g["OpenTriRight"]  << "'>'";

    g["Line"] << "'-'* '[hidden]'? '-'*";

    g["RelationshipLabel"] << "Colon '<'? Name* '>'?";

    g["ExtensionLeft"]    << "Identifier QuotedName? TriangleLeft Line QuotedName? Identifier RelationshipLabel?";
    g["CompositionLeft"]  << "Identifier QuotedName? Composition Line QuotedName? Identifier RelationshipLabel?";
    g["AggregationLeft"]  << "Identifier QuotedName? Aggregation Line QuotedName? Identifier RelationshipLabel?";
    g["UsageLeft"]        << "Identifier QuotedName? OpenTriLeft Line QuotedName? Identifier RelationshipLabel?";
    g["RelationshipLeft"] << "ExtensionLeft | CompositionLeft | AggregationLeft | UsageLeft";

    g["ExtensionRight"]    << "Identifier QuotedName? Line TriangleRight QuotedName? Identifier RelationshipLabel?";
    g["CompositionRight"]  << "Identifier QuotedName? Line Composition QuotedName? Identifier RelationshipLabel?";
    g["AggregationRight"]  << "Identifier QuotedName? Line Aggregation QuotedName? Identifier RelationshipLabel?";
    g["UsageRight"]        << "Identifier QuotedName? Line OpenTriRight QuotedName? Identifier RelationshipLabel?";
    g["RelationshipRight"] << "ExtensionRight | CompositionRight | AggregationRight | UsageRight";

    g["Relationship"] << "RelationshipLeft | RelationshipRight";


    // ========= NAMESPACES =========
    g["Package"] << "'package' Name Color? OpenBrackets ((Container | Relationship | Ignored)? Endl)* CloseBrackets"
            >> [](auto e, auto& p) { 
                p.namespaces.push_back(PlantUML{e[0].string()});
                std::for_each(e.begin(), e.end(), [&n=p.namespaces.back()] (auto e) {
                    e.evaluate(n);
                });
            };


    // ========= DIAGRAM =========
    g["Body"] << "((Container | Relationship | ExternalDefinitions | Package | Ignored)? Endl)*";
    g["Start"] << "'@startuml' Name? Endl";
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