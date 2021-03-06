#include "Parser.h"

#include <algorithm>
#include <iostream>

Parser::Parser(/* args */)
{
    // For reference you can get plantuml regexes and format them to work with std::regex in bash:
    // java -jar plantuml.jar -pattern | sed '/^net./d' | sed '/^No/d' | sed 's/\\/\\\\/g' | sed 's/"/\\"/g'

    // ========= GENERAL KEYWORDS =========
    g.setSeparator(g["Whitespace"] << "[\t ]");
    g["Identifier"] << "[a-zA-Z.:] [a-zA-Z0-9_.:]*";
    g.setProgramRule("QuotedName", peg_parser::presets::createStringProgram("\"", "\""), [](auto e, auto& v) {});
    g["Name"] << "Identifier | QuotedName";
    g["FieldTypename"] << "[a-zA-Z] [a-zA-Z0-9_.:<>]* '[]'?";

    g["ColorName"] << "'#' (!(' ' | ')' | '\n') .)* | Identifier";
    g["Gradient"] << "'|' | '/' | '-' | '\\\\'";
    g["Color"] << "ColorName (Gradient ColorName)?";

    // stereotypes
    g["Spot"] << "'(' . ',' Color ')'";
    g["Stereotype"] << "'<<' Spot? Identifier? '>>'" >> [](auto e, auto& v) { v.visitStereotype(e["Identifier"]); };

    g["Endl"] << "'\n'";
    g["OpenBrackets"] << "Endl? '{'";
    g["CloseBrackets"] << "'}'";

    // ========= COMMENTS =========
    g["Comment"] << "'\\'' (!'\n' .)*";
    g["Include"] << "'!include' (!'\n' .)*"; // ignore !include

    g["Ignored"] << "Comment | Include";

    // ========= CATCHALL =========
    g["CatchAll"] << "(!'\n' .)*" >> [](auto e, auto&) { std::cout << "unable to parse line:\n" << e.view(); };

    // ========= ELEMENTS =========
    g["Private"] << "'-'" >> [](auto e, auto& v) { v.visitPrivateVisibility(); };
    g["Protected"] << "'#'" >> [](auto e, auto& v) { v.visitProtectedVisibility(); };
    g["PackagePrivate"] << "'~'" >> [](auto e, auto& v) { v.visitPackagePrivateVisibility(); };
    g["Public"] << "'+'" >> [](auto e, auto& v) { v.visitPublicVisibility(); };
    g["Visibility"] << "Private | Protected | PackagePrivate | Public";

    // fields
    g["FieldDefExplicit"] << "'{field}' Identifier+";
    g["FieldDefColon"] << "Visibility? Identifier ':' FieldTypename" >>
        [](auto e, auto& v) { v.visitField(*e["FieldTypename"], *e["Identifier"], e["Visibility"]); };
    g["FieldDefImplicit"] << "Visibility? FieldTypename Identifier" >>
        [](auto e, auto& v) { v.visitField(*e["FieldTypename"], *e["Identifier"], e["Visibility"]); };
    g["FieldDefTypeOnly"] << "Visibility? FieldTypename" >>
        [](auto e, auto& v) { v.visitField(*e["FieldTypename"], *e["FieldTypename"], e["Visibility"]); };
    g["FieldDef"] << "FieldDefExplicit | FieldDefColon | FieldDefImplicit | "
                     "FieldDefTypeOnly";

    // parameter list
    g["Parameter"] << "Identifier ':' FieldTypename | FieldTypename Identifier" >>
        [](auto e, auto& v) { v.visitParameter(*e["FieldTypename"], *e["Identifier"]); };
    g["ParamList"] << "'(' (Parameter (',' Parameter)*)? ')'";

    // methods
    g["MethodDefExplicit"] << "'{method}' Identifier+";
    g["MethodDefTrailingReturn"] << "Visibility? Identifier ParamList (':' FieldTypename)?" >>
        [](auto e, auto& v) { v.visitMethod(*e["Identifier"], *e["ParamList"], e["FieldTypename"], e["Visibility"]); };
    g["MethodDefLeadingReturn"] << "Visibility? FieldTypename Identifier ParamList" >>
        [](auto e, auto& v) { v.visitMethod(*e["Identifier"], *e["ParamList"], e["FieldTypename"], e["Visibility"]); };
    g["MethodDef"] << "MethodDefExplicit | MethodDefLeadingReturn | "
                      "MethodDefTrailingReturn";

    // external fields and methods
    g["ExtFieldDef"] << "Identifier ':' FieldDef" >> [](auto e, auto& v) { v.visitExternalField(e[0], e[1]); };
    g["ExtMethodDef"] << "Identifier ':' MethodDef" >> [](auto e, auto& v) { v.visitExternalMethod(e[0], e[1]); };
    g["ExternalDefinitions"] << "ExtMethodDef | ExtFieldDef";

    // ========= RELATIONSHIPS =========
    g["TriangleLeft"] << "'<|'";
    g["TriangleRight"] << "'|>'";
    g["Composition"] << "'*'";
    g["Aggregation"] << "'o'";
    g["OpenTriLeft"] << "'<'";
    g["OpenTriRight"] << "'>'";

    g["Line"] << "'-'* '[hidden]'? '-'*";

    g["Label"] << "(!('>' | '\n') .)*";

    g["Object"] << "Identifier";
    g["Subject"] << "Identifier";
    g["Cardinality"] << "QuotedName";

    g["ExtensionSubjectLeft"] << "Line TriangleRight" >> [](auto e, auto& v) { v.visitExtension(); };
    g["CompositionSubjectLeft"] << "Composition Line" >> [](auto e, auto& v) { v.visitComposition(); };
    g["AggregationSubjectLeft"] << "Aggregation Line" >> [](auto e, auto& v) { v.visitAggregation(); };
    g["UsageSubjectLeft"] << "Line OpenTriRight" >> [](auto e, auto& v) { v.visitUsage(); };

    g["ConnectorLeft"] << "ExtensionSubjectLeft | CompositionSubjectLeft | "
                          "AggregationSubjectLeft | UsageSubjectLeft";
    g["RelationshipLeftSubject"] << "Subject QuotedName? ConnectorLeft "
                                    "Cardinality? Object (':' '<'? Label '>'?)?" >>
        [](auto e, auto& v) {
            v.visitRelationship(
                *e["Subject"], *e["ConnectorLeft"], *e["Object"], e["Cardinality"], e["QuotedName"], e["Label"]);
        };

    g["ExtensionSubjectRight"] << "TriangleLeft Line" >> [](auto e, auto& v) { v.visitExtension(); };
    g["CompositionSubjectRight"] << "Line Composition" >> [](auto e, auto& v) { v.visitComposition(); };
    g["AggregationSubjectRight"] << "Line Aggregation" >> [](auto e, auto& v) { v.visitAggregation(); };
    g["UsageSubjectRight"] << "OpenTriLeft Line" >> [](auto e, auto& v) { v.visitUsage(); };

    g["ConnectorRight"] << "ExtensionSubjectRight | CompositionSubjectRight | "
                           "AggregationSubjectRight | UsageSubjectRight";
    g["RelationshipRightSubject"] << "Object Cardinality? ConnectorRight "
                                     "QuotedName? Subject (':' '<'? Label '>'?)?" >>
        [](auto e, auto& v) {
            v.visitRelationship(
                *e["Subject"], *e["ConnectorRight"], *e["Object"], e["Cardinality"], e["QuotedName"], e["Label"]);
        };

    g["Relationship"] << "RelationshipLeftSubject | RelationshipRightSubject";

    // ========= CONTAINERS =========
    g["ClassType"] << "'abstract' 'class'? | 'class' | 'entity' | 'interface'";
    g["IgnoredType"] << "'annotation' | 'circle' | '()' | 'diamond' | '<>'";

    // body of containers
    g["ClassBody"] << "OpenBrackets ((MethodDef | FieldDef | "
                      "Ignored | CatchAll)? '\n')* CloseBrackets";
    g["EnumBody"] << "OpenBrackets (Identifier? '\n')* CloseBrackets";

    // simple containers
    g["Class"] << "ClassType Name (Color | Stereotype)? ClassBody?" >>
        [](auto e, auto& v) { v.visitClass(*e["ClassType"], *e["Name"], e["Stereotype"], e["ClassBody"]); };
    g["Enum"] << "'enum' Name (Color | Stereotype)? EnumBody?" >>
        [](auto e, auto& v) { v.visitEnum(*e["Name"], e["EnumBody"]); };
    g["IgnoredDef"] << "IgnoredType Name (Color | Stereotype)?";

    // collector rule
    g["Container"] << "ClassWithBody | EnumWithBody | Class | Enum | IgnoredDef";

    // ========= SETTERS =========
    g["Set"] << "'set'";
    g["Separator"] << "(!'\n' .)*";
    g["SetNamespace"] << "Set 'namespaceSeparator' Separator" >>
        [](auto e, auto& v) { v.visitSetNamespaceSeparator(e[1]); };
    g["Setter"] << "SetNamespace";

    // ========= NAMESPACES =========
    g["Body"] << "((Setter | Container | Relationship | ExternalDefinitions | "
                 "Package | Ignored | CatchAll)? '\n')*";
    g["NamespaceDef"] << "'namespace' Name Color? OpenBrackets Body CloseBrackets" >>
        [](auto e, auto& v) { v.visitNamespace(*e["Name"], *e["Body"]); };
    g["PackageDef"] << "'package' Name (Color | Stereotype)? OpenBrackets Body CloseBrackets" >>
        [](auto e, auto& v) { v.visitPackage(*e["Name"], *e["Body"]); };
    g["Package"] << "PackageDef | NamespaceDef";

    // ========= DIAGRAM =========
    g["Start"] << "'@startuml' Name? '\n'" >> [](auto e, auto& v) { v.visitStart(e["Name"]); };
    g["End"] << "'@enduml' '\n'?";
    g["Diagram"] << "Start Body End";

    g.setStart(g["Diagram"]);
}

bool Parser::parse(std::string_view input)
{
    m_ast = g.parse(input);
    return m_ast->valid && m_ast->end >= input.size();
}

void Parser::visitAST(AbstractVisitor& visitor)
{
    g.interpret(m_ast).evaluate(visitor);
}