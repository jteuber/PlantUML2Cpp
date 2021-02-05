#include "Parser.h"

#include <algorithm>
#include <iostream>

Parser::Parser(/* args */)
{
    // ========= GENERAL KEYWORDS =========
    g.setSeparator(g["Whitespace"] << "[\t ]");
    g["Identifier"] << "[a-zA-Z.] [a-zA-Z0-9_.]*";
    g.setProgramRule("QuotedName", peg_parser::presets::createStringProgram("\"", "\""),
                     [](auto e, auto &v) {});
    g["Name"] << "Identifier | QuotedName" >> [](auto e, auto &v) { v.visitName(e); };
    g["FieldTypename"] << "[a-zA-Z] [a-zA-Z0-9_.:<>]* '[]'?";

    g["Color"] << "'#' (!(' ' | ')' | '\n') .)* | Identifier";

    g["Endl"] << "'\n'";
    g["OpenBrackets"] << "Endl? '{'" >> [](auto e, auto &v) { v.visitOpeningBracket(); };
    g["CloseBrackets"] << "'}'" >> [](auto e, auto &v) { v.visitClosingBracket(); };

    // ========= COMMENTS =========
    g["Comment"] << "'\\'' (!'\n' .)*";
    g["Include"] << "'!include' (!'\n' .)*"; // ignore !include

    g["Ignored"] << "Comment | Include";

    // ========= ELEMENTS =========
    // fields
    g["FieldDefExplicit"] << "'{field}' Identifier+" >> [](auto e, auto &v) {};
    g["FieldDefColon"] << "Identifier ':' FieldTypename" >> [](auto e, auto &v) { v.visitField(e[1], e[0]); };
    g["FieldDefImplicit"] << "FieldTypename Identifier" >> [](auto e, auto &v) { v.visitField(e[0], e[1]); };
    g["FieldDefTypeOnly"] << "FieldTypename" >> [](auto e, auto &v) { v.visitField(e[0], e[0]); };
    g["FieldDef"] << "FieldDefExplicit | FieldDefColon | FieldDefImplicit | FieldDefTypeOnly";

    // parameter list
    g["ParamList"] << "'(' (FieldDef (',' FieldDef)*)? ')'";

    // methods
    g["MethodDefExplicit"] << "'{method}' Identifier+";
    g["MethodDefTrailingReturn"] << "Identifier ParamList ':' FieldTypename" >> [](auto e, auto &v) { v.visitMethod(e[2], e[0], e[1]); };
    g["MethodDefLeadingReturn"] << "FieldTypename Identifier ParamList" >> [](auto e, auto &v) { v.visitMethod(e[0], e[1], e[2]); };
    g["MethodDefNoReturn"] << "Identifier ParamList" >> [](auto e, auto &v) { v.visitVoidMethod(e[0], e[1]); };
    g["MethodDef"] << "MethodDefExplicit | MethodDefTrailingReturn | MethodDefLeadingReturn | MethodDefNoReturn";

    // external fields and methods
    g["ExtFieldDef"] << "Identifier ':' FieldDef" >> [](auto e, auto &v) { v.visitExternalField(e[0], e[1]); };
    g["ExtMethodDef"] << "Identifier ':' MethodDef" >> [](auto e, auto &v) { v.visitExternalMethod(e[0], e[1]); };
    g["ExternalDefinitions"] << "ExtMethodDef | ExtFieldDef";

    // ========= RELATIONSHIPS =========
    g["TriangleLeft"] << "'<|'";
    g["TriangleRight"] << "'|>'";
    g["Composition"] << "'*'";
    g["Aggregation"] << "'o'";
    g["OpenTriLeft"] << "'<'";
    g["OpenTriRight"] << "'>'";

    g["Line"] << "'-'* '[hidden]'? '-'*";

    g["Label"] << "(!('>' | '\n') .)*" >> [](auto e, auto &v) { v.visitRelationshipLabel(e); };
    g["RelationshipLabel"] << "':' '<'? Label '>'?";

    g["Cardinality"] << "QuotedName" >> [](auto e, auto &v) { v.visitCardinality(e[0]); };

    g["ExtensionSubjectLeft"] << "Line TriangleRight";
    g["CompositionSubjectLeft"] << "Composition Line";
    g["AggregationSubjectLeft"] << "Aggregation Line";
    g["UsageSubjectLeft"] << "Line OpenTriRight";

    g["ObjectRight"] << "Cardinality? Identifier" >> [](auto e, auto &v) { auto s = e.size(); v.visitObject(e[s-1]); if(s > 1) e[0].evaluate(v); };
    g["ExtensionObjectPartRight"] << "QuotedName? ExtensionSubjectLeft ObjectRight" >> [](auto e, auto &v) { auto s = e.size(); v.visitExtension(e[s-1]); };
    g["CompositionObjectPartRight"] << "QuotedName? CompositionSubjectLeft ObjectRight" >> [](auto e, auto &v) { auto s = e.size(); v.visitComposition(e[s-1]); };
    g["AggregationObjectPartRight"] << "QuotedName? AggregationSubjectLeft ObjectRight" >> [](auto e, auto &v) { auto s = e.size(); v.visitAggregation(e[s-1]); };
    g["UsageObjectPartRight"] << "QuotedName? UsageSubjectLeft ObjectRight" >> [](auto e, auto &v) { auto s = e.size(); v.visitUsage(e[s-1]); };
    g["RelationshipObjectPartRight"] << "ExtensionObjectPartRight | CompositionObjectPartRight | AggregationObjectPartRight | UsageObjectPartRight";
    g["RelationshipLeftSubject"] << "Identifier RelationshipObjectPartRight RelationshipLabel?" >> [](auto e, auto &v) { v.visitRelationship(e[0], e[1], e["RelationshipLabel"]); };

    g["ExtensionSubjectRight"] << "TriangleLeft Line";
    g["CompositionSubjectRight"] << "Line Composition";
    g["AggregationSubjectRight"] << "Line Aggregation";
    g["UsageSubjectRight"] << "OpenTriLeft Line";

    g["ObjectLeft"] << "Identifier Cardinality?" >> [](auto e, auto &v) { v.visitObject(e[0]); if(e.size() > 1) e[1].evaluate(v); };
    g["ExtensionObjectPartLeft"] << "ObjectLeft ExtensionSubjectRight QuotedName?" >> [](auto e, auto &v) { v.visitExtension(e[0]); };
    g["CompositionObjectPartLeft"] << "ObjectLeft CompositionSubjectRight QuotedName?" >> [](auto e, auto &v) { v.visitComposition(e[0]); };
    g["AggregationObjectPartLeft"] << "ObjectLeft AggregationSubjectRight QuotedName?" >> [](auto e, auto &v) { v.visitAggregation(e[0]); };
    g["UsageObjectPartLeft"] << "ObjectLeft UsageSubjectRight QuotedName?" >> [](auto e, auto &v) { v.visitUsage(e[0]); };
    g["RelationshipObjectPartLeft"] << "ExtensionObjectPartLeft | CompositionObjectPartLeft | AggregationObjectPartLeft | UsageObjectPartLeft";
    g["RelationshipRightSubject"] << "RelationshipObjectPartLeft Identifier RelationshipLabel?" >> [](auto e, auto &v) { v.visitRelationship(e[1], e[0], e["RelationshipLabel"]); };

    g["Relationship"] << "RelationshipLeftSubjectFull | RelationshipRightSubjectFull | RelationshipLeftSubject | RelationshipRightSubject";

    // ========= CONTAINERS =========
    // keywords
    g["KW_Class"] << "'class'";
    g["KW_Abstract"] << "'abstract' KW_Class?";
    g["KW_Circle"] << "'circle' | '()'";
    g["KW_Diamond"] << "'diamond' | '<>'";

    // stereotypes
    g["Spot"] << "'(' . ',' Color ')'";
    g["Stereotype"] << "'<<' Spot? Identifier? '>>'";

    // simple containers
    g["Abstract"] << "KW_Abstract Name Stereotype?" >> [](auto e, auto &v) { v.visitContainer(e, PlantUML::Type::Abstract); };
    g["Annotation"] << "'annotation' Name Stereotype?" >> [](auto e, auto &v) { v.visitContainer(e, PlantUML::Type::Annotation); };
    g["Circle"] << "KW_Circle Name Stereotype?" >> [](auto e, auto &v) { v.visitContainer(e, PlantUML::Type::Circle); };
    g["Class"] << "'class' Name Stereotype?" >> [](auto e, auto &v) { v.visitContainer(e, PlantUML::Type::Class); };
    g["Diamond"] << "KW_Diamond Name Stereotype?" >> [](auto e, auto &v) { v.visitContainer(e, PlantUML::Type::Diamond); };
    g["Entity"] << "'entity' Name Stereotype?" >> [](auto e, auto &v) { v.visitContainer(e, PlantUML::Type::Entity); };
    g["Enum"] << "'enum' Name Stereotype?" >> [](auto e, auto &v) { v.visitContainer(e, PlantUML::Type::Enum); };
    g["Interface"] << "'interface' Name Stereotype?" >> [](auto e, auto &v) { v.visitContainer(e, PlantUML::Type::Interface); };

    // containers with bodies
    g["ClassWithBody"] << "Class OpenBrackets ((MethodDef | FieldDef | Ignored)? '\n')* CloseBrackets";
    g["EntityWithBody"] << "Entity OpenBrackets ((FieldDef | Ignored)? '\n')* CloseBrackets";
    g["InterfaceWithBody"] << "Interface OpenBrackets ((MethodDef | Ignored)? '\n')* CloseBrackets";
    g["EnumWithBody"] << "Enum OpenBrackets (Identifier? '\n')* CloseBrackets";

    // collector rule
    g["Container"] << "ClassWithBody | EntityWithBody | InterfaceWithBody | EnumWithBody | Abstract | Annotation | Circle | Class | Diamond | Entity | Enum | Interface ";

    // ========= NAMESPACES =========
    g["Body"] << "((Container | Relationship | ExternalDefinitions | Package | Ignored)? '\n')*";
    g["NamespaceDef"] << "'namespace' Name Color?" >> [](auto e, auto &v) { v.visitContainer(e, PlantUML::Type::Namespace); };
    g["PackageDef"] << "'package' Name Color?" >> [](auto e, auto &v) { v.visitContainer(e, PlantUML::Type::Package); };
    g["Package"] << "(PackageDef | NamespaceDef) OpenBrackets Body CloseBrackets";

    // ========= DIAGRAM =========
    g["Start"] << "'@startuml' Name? '\n'" >> [](auto e, auto &v) { v.visitStart(e["Name"]); };
    g["End"] << "'@enduml' '\n'?";
    g["Diagram"] << "Start Body End";

    g.setStart(g["Diagram"]);
}

Parser::~Parser()
{
}

PlantUMLPtr Parser::parse(const std::string &input)
{
    Visitor visitor;
    g.run(input, visitor);
    return visitor.getResult();
}