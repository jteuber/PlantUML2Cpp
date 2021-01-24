#include "Parser.h"

#include <iostream>
#include <algorithm>

Parser::Parser(/* args */)
{
    // ========= GENERAL KEYWORDS =========
    g.setSeparator(g["Whitespace"] << "[\t ]");
    g["Identifier"] << "[a-zA-Z] [a-zA-Z0-9_.]*";
    g.setProgramRule("QuotedName", peg_parser::presets::createStringProgram("\"", "\""),
                   [] (auto e, auto& v) { });
    g["Name"] << "Identifier | QuotedName" >> [] (auto e, auto& v) { v.visitName(e); };
    g["FieldTypename"] << "[a-zA-Z] [a-zA-Z0-9_.:<>]* '[]'?";

    g["Color"]  << "'#' (!(' ' | ')' | '\n') .)* | Identifier";

    g["Endl"] << "'\n'";
    g["OpenBrackets"]  << "Endl? '{'" >> [] (auto e, auto& v) { v.visitOpeningBracket(); };
    g["CloseBrackets"] << "'}'" >> [] (auto e, auto& v) { v.visitClosingBracket(); };

    // ========= COMMENTS =========
    g["Comment"] << "'\\'' (!'\n' .)*";
    g["Include"] << "'!include' (!'\n' .)*"; // ignore !include

    g["Ignored"] << "Comment | Include";

    // ========= ELEMENTS =========
    // fields, parameters and methods
    g["FieldDefExplicit"] << "'{field}' Identifier+"        >> [] (auto e, auto& v) {};
    g["FieldDefColon"]    << "Identifier ':' FieldTypename" >> [] (auto e, auto& v) { v.visitField(e[1], e[0]); };
    g["FieldDefImplicit"] << "FieldTypename Identifier"     >> [] (auto e, auto& v) { v.visitField(e[0], e[1]); };
    g["FieldDefTypeOnly"] << "FieldTypename"                >> [] (auto e, auto& v) { v.visitField(e[0], e[0]); };
    g["FieldDef"] << "FieldDefExplicit | FieldDefColon | FieldDefImplicit | FieldDefTypeOnly";


    g["ParamList"] << "'(' (FieldDef (',' FieldDef)*)? ')'";
    g["MethodDef"] << "'{method}' Identifier+ | Identifier ParamList (':' FieldTypename)? | FieldTypename Identifier ParamList";

    // external fields and methods
    g["ExtFieldDef"] << "Identifier ':' FieldDef" >> [] (auto e, auto& v) { v.visitExternalField(e[0], e[1]); };
    g["ExtMethodDef"] << "Identifier ':' MethodDef";
    g["ExternalDefinitions"] << "ExtMethodDef | ExtFieldDef";


    // ========= RELATIONSHIPS =========
    g["TriangleLeft"]  << "'<|'";
    g["TriangleRight"] << "'|>'";
    g["Composition"]   << "'*'";
    g["Aggregation"]   << "'o'";
    g["OpenTriLeft"]   << "'<'";
    g["OpenTriRight"]  << "'>'";

    g["Line"] << "'-'* '[hidden]'? '-'*";

    g["RelationshipLabel"] << "':' '<'? (!('>' | '\n') .)* '>'?";

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
    g["Abstract"]   << "KW_Abstract Name Stereotype?"  >> [] (auto e, auto& v) { v.visitContainer(e, PlantUML::Type::Abstract); };
    g["Annotation"] << "'annotation' Name Stereotype?" >> [] (auto e, auto& v) { v.visitContainer(e, PlantUML::Type::Annotation); };
    g["Circle"]     << "KW_Circle Name Stereotype?"    >> [] (auto e, auto& v) { v.visitContainer(e, PlantUML::Type::Circle); };
    g["Class"]      << "'class' Name Stereotype?"      >> [] (auto e, auto& v) { v.visitContainer(e, PlantUML::Type::Class); };
    g["Diamond"]    << "KW_Diamond Name Stereotype?"   >> [] (auto e, auto& v) { v.visitContainer(e, PlantUML::Type::Diamond); };
    g["Entity"]     << "'entity' Name Stereotype?"     >> [] (auto e, auto& v) { v.visitContainer(e, PlantUML::Type::Entity); };
    g["Enum"]       << "'enum' Name Stereotype?"       >> [] (auto e, auto& v) { v.visitContainer(e, PlantUML::Type::Enum); };
    g["Interface"]  << "'interface' Name Stereotype?"  >> [] (auto e, auto& v) { v.visitContainer(e, PlantUML::Type::Interface); };
    // TODO: stereotypes << ... >>

    // containers with bodies
    g["ClassWithBody"]     << "Class OpenBrackets ((MethodDef | FieldDef | Ignored)? '\n')* CloseBrackets";
    g["EntityWithBody"]    << "Entity OpenBrackets ((FieldDef | Ignored)? '\n')* CloseBrackets";
    g["InterfaceWithBody"] << "Interface OpenBrackets ((MethodDef | Ignored)? '\n')* CloseBrackets";
    g["EnumWithBody"]      << "Enum OpenBrackets (Identifier? '\n')* CloseBrackets";
    
    // collector rule
    g["Container"] << "ClassWithBody | EntityWithBody | InterfaceWithBody | EnumWithBody | Abstract | Annotation | Circle | Class | Diamond | Entity | Enum | Interface ";



    // ========= NAMESPACES =========
    g["Body"] << "((Container | Relationship | ExternalDefinitions | Package | Ignored)? '\n')*";
    g["PackageDef"] << "'package' Name Color?" >> [] (auto e, auto& v) { v.visitContainer(e, PlantUML::Type::Namespace); };
    g["Package"] << "PackageDef OpenBrackets Body CloseBrackets";


    // ========= DIAGRAM =========
    g["Start"] << "'@startuml' Name? '\n'";
    g["End"] << "'@enduml' '\n'?";
    g["Diagram"] << "Start Body End" >> [] (auto e, auto& v) { v.visitDiagram(e[0], e[1]); };
    
    g.setStart(g["Diagram"]);
}

Parser::~Parser()
{
}

PlantUML Parser::parse(const std::string& input) 
{
    Visitor visitor;
    g.run(input, visitor);
    return visitor.getResult();
}