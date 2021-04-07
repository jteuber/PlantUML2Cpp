#include "PlantUml/Parser.h"

#include "PlantUml/ModelElement.h"

#include <algorithm>
#include <bits/ranges_algobase.h>
#include <iostream>
#include <iterator>
#include <stack>
#include <variant>

using namespace std::literals;

namespace PlantUml {

Parser::Parser(/* args */)
{
    // For reference you can get plantuml regexes and format them to work with std::regex in bash:
    // java -jar plantuml.jar -pattern | sed '/^net./d' | sed '/^No/d' | sed 's/\\/\\\\/g' | sed 's/"/\\"/g'

    // ========= GENERAL KEYWORDS =========
    g.setSeparator(g["Whitespace"] << "[\t ]");
    g["Identifier"] << "[a-zA-Z.:] [a-zA-Z0-9_.:]*";
    g.setProgramRule("QuotedName", peg_parser::presets::createStringProgram("\"", "\""), [](auto e) {
        return SyntaxNode{e.string()};
    });
    g["Name"] << "Identifier | QuotedName";
    g["SimpleType"] << "[a-zA-Z] [a-zA-Z0-9_.:]* '&'?";
    g["FieldTypename"] << "SimpleType '<' FieldTypename (',' (FieldTypename | [0-9]+))* '>' | SimpleType '[]'?";

    g["ColorName"] << "'#' (!(' ' | ')' | Endl) .)* | Identifier";
    g["Gradient"] << "'|' | '/' | '-' | '\\\\'";
    g["Color"] << "ColorName (Gradient ColorName)?";

    // stereotypes
    g["Spot"] << "'(' . ',' Color ')'";
    g["Stereotype"] << "'<<' Spot? Identifier? '>>'"; // >> [](auto e) { v.visitStereotype(e["Identifier"]); };

    g["Endl"] << "'\r\n' | '\n'" >> [](auto /*e*/) { return SyntaxNode{std::string()}; };
    g["OpenBrackets"] << "Endl? '{'";
    g["CloseBrackets"] << "'}'";

    // Visibilities
    g["Private"] << "'-'" >> [](auto /*e*/) { return SyntaxNode{Visibility::Private}; };
    g["Protected"] << "'#'" >> [](auto /*e*/) { return SyntaxNode{Visibility::Protected}; };
    g["PackagePrivate"] << "'~'" >> [](auto /*e*/) { return SyntaxNode{Visibility::PackagePrivate}; };
    g["Public"] << "'+'" >> [](auto /*e*/) { return SyntaxNode{Visibility::Public}; };
    g["Visibility"] << "Private | Protected | PackagePrivate | Public";

    // ========= COMMENTS =========
    g["Comment"] << "'\\'' (!Endl .)*";
    g["Include"] << "'!include' (!Endl .)*"; // ignore !include

    g["Ignored"] << "Comment | Include" >> [](auto /*e*/) { return SyntaxNode{std::string()}; };

    // ========= WARNINGS =========
    g["WARN_Unrecognized_Line"] << "!(End | CloseBrackets) (!Endl .)+" >> [this](auto e) {
        auto lineStartIt = std::ranges::upper_bound(newLinePositions, e.position()) - 1;
        auto lineNr      = std::distance(newLinePositions.begin(), lineStartIt) + 1;
        std::cout << "WARNING! Line " << lineNr << ": Unrecognized line: " << e.view() << std::endl;
        return SyntaxNode{std::string()};
    };
    g["WARN_Extepted_EOL"] << "!(End | CloseBrackets) (!Endl .)+" >> [this](auto e) {
        auto lineStartIt = std::ranges::upper_bound(newLinePositions, e.position()) - 1;
        auto lineNr      = std::distance(newLinePositions.begin(), lineStartIt) + 1;
        std::cout << "WARNING! Line " << lineNr << ": Expected end of line before \"" << e.view() << "\"" << std::endl;
        return SyntaxNode{std::string()};
    };

    // // ========= SETTERS =========
    g["Set"] << "'set'";
    g["Separator"] << "(!Endl .)*";
    g["SetNamespace"] << "Set 'namespaceSeparator' Separator" >> [this](auto e) {
        namespaceDelimiter = e["Separator"]->string();
        return SyntaxNode{std::string()};
    };
    g["Setter"] << "SetNamespace";

    // ========= PARAMETER =========
    g["Parameter"] << "Identifier ':' FieldTypename | FieldTypename Identifier" >> [this](auto e) {
        return SyntaxNode{Parameter{toName(e["Identifier"]), toNamespace(e["FieldTypename"])}};
    };

    // parameter list
    g["ParamList"] << "'(' (Parameter (',' Parameter)*)? ')'" >> [this](auto e) { return evaluateBody(e); };

    // ========= NOTE =========
    // TODO

    // ========= SEPARATOR =========
    // TODO

    // ========= ENUMERATOR =========
    g["Enumerator"] << "Identifier" >> [](auto e) { return SyntaxNode{Enumerator{toName(e["Identifier"])}}; };

    // ========= RELATIONSHIPS =========
    g["TriangleLeft"] << "'<|'";
    g["TriangleRight"] << "'|>'";
    g["Composition"] << "'*'";
    g["Aggregation"] << "'o'";
    g["OpenTriLeft"] << "'<'";
    g["OpenTriRight"] << "'>'";

    g["LineModifiers"] << "'[hidden]' | 'left' | 'right' | 'up' | 'down'";
    g["Line"] << "'-'* LineModifiers? '-'*";

    g["Label"] << "(!('>' | Endl) .)*";

    g["Object"] << "Identifier";
    g["Subject"] << "Identifier";
    g["Cardinality"] << "QuotedName";

    g["ExtensionSubjectLeft"] << "Line TriangleRight" >> [](auto e) {
        Relationship r;
        r.type = RelationshipType::Extension;
        return SyntaxNode{r};
    };
    g["CompositionSubjectLeft"] << "Composition Line" >> [](auto e) {
        Relationship r;
        r.type = RelationshipType::Composition;
        return SyntaxNode{r};
    };
    g["AggregationSubjectLeft"] << "Aggregation Line" >> [](auto e) {
        Relationship r;
        r.type = RelationshipType::Aggregation;
        return SyntaxNode{r};
    };
    g["UsageSubjectLeft"] << "Line OpenTriRight" >> [](auto e) {
        Relationship r;
        r.type = RelationshipType::Usage;
        return SyntaxNode{r};
    };

    g["ExtensionSubjectRight"] << "TriangleLeft Line" >> [](auto e) {
        Relationship r;
        r.type = RelationshipType::Extension;
        return SyntaxNode{r};
    };
    g["CompositionSubjectRight"] << "Line Composition" >> [](auto e) {
        Relationship r;
        r.type = RelationshipType::Composition;
        return SyntaxNode{r};
    };
    g["AggregationSubjectRight"] << "Line Aggregation" >> [](auto e) {
        Relationship r;
        r.type = RelationshipType::Aggregation;
        return SyntaxNode{r};
    };
    g["UsageSubjectRight"] << "OpenTriLeft Line" >> [](auto e) {
        Relationship r;
        r.type = RelationshipType::Usage;
        return SyntaxNode{r};
    };
    g["ConnectorRight"]
        << "ExtensionSubjectRight | CompositionSubjectRight | AggregationSubjectRight | UsageSubjectRight";
    g["ConnectorLeft"] << "ExtensionSubjectLeft | CompositionSubjectLeft | AggregationSubjectLeft | UsageSubjectLeft";

    g["Relationship"] << "Object Cardinality? ConnectorRight QuotedName? Subject (':' '<'? Label '>'?)? | "
                         "Subject QuotedName? ConnectorLeft Cardinality? Object (':' '<'? Label '>'?)?" >>
        [this](auto e) {
            SyntaxNode n;
            if (e["ConnectorRight"])
                n = e["ConnectorRight"]->evaluate();
            else
                n = e["ConnectorLeft"]->evaluate();
            auto& r              = std::get<Relationship>(n.element);
            r.subject            = toNamespace(e["Subject"]);
            r.object             = toNamespace(e["Object"]);
            r.subjectCardinality = toName(e["QuotedName"]);
            r.objectCardinality  = toName(e["Cardinality"]);
            r.label              = toName(e["Label"]);
            r.hidden             = false; // TODO
            return n;
        };

    // ========= VARIABLE =========
    g["VariableExplicit"] << "'{field}' Identifier+";
    g["VariableImplicit"] << "Visibility? Identifier ':' FieldTypename | Visibility? FieldTypename Identifier?" >>
        [this](auto e) {
            Variable var = Variable{toName(e["Identifier"]), toNamespace(e["FieldTypename"])};
            var.visibility =
                e["Visibility"] ? std::get<Visibility>(e["Visibility"]->evaluate().element) : Visibility::Unspecified;
            return SyntaxNode{var};
        };
    g["Variable"] << "VariableExplicit | VariableImplicit";

    // external variable definition
    g["ExternalVariable"] << "Identifier ':' Variable" >> [this](auto e) {
        auto n    = e["Variable"]->evaluate();
        auto v    = std::get<Variable>(n.element);
        v.element = toNamespace(e["Identifier"]);
        return SyntaxNode{v};
    };

    // ========= METHOD =========
    g["MethodExplicit"] << "'{method}' Identifier+";
    g["MethodImplicit"] << "Visibility? FieldTypename Identifier ParamList | Visibility? Identifier ParamList (':' "
                           "FieldTypename)?" >>
        [this](auto e) {
            Method m = Method{toName(e["Identifier"]), toNamespace(e["FieldTypename"])};
            m.visibility =
                e["Visibility"] ? std::get<Visibility>(e["Visibility"]->evaluate().element) : Visibility::Unspecified;
            auto n     = SyntaxNode{m};
            n.children = std::move(e["ParamList"]->evaluate().children);
            return n;
        };
    g["Method"] << "MethodExplicit | MethodImplicit";

    // external variable definition
    g["ExternalMethod"] << "Identifier ':' Method" >> [this](auto e) {
        auto n    = e["Method"]->evaluate();
        auto m    = std::get<Method>(n.element);
        m.element = toNamespace(e["Identifier"]);
        return SyntaxNode{m};
    };

    // ========= ELEMENT =========
    g["AbstractType"] << "'abstract class' | 'abstract'" >>
        [](auto /*e*/) { return SyntaxNode{ElementType::Abstract}; };
    g["AnnotationType"] << "'annotation'" >> [](auto /*e*/) { return SyntaxNode{ElementType::Annotation}; };
    g["ClassType"] << "'class'" >> [](auto /*e*/) { return SyntaxNode{ElementType::Class}; };
    g["EntityType"] << "'entity'" >> [](auto /*e*/) { return SyntaxNode{ElementType::Entity}; };
    g["EnumType"] << "'enum'" >> [](auto /*e*/) { return SyntaxNode{ElementType::Enum}; };
    g["InterfaceType"] << "'interface'" >> [](auto /*e*/) { return SyntaxNode{ElementType::Interface}; };

    g["ElementType"] << "AbstractType | AnnotationType | ClassType | EntityType | EnumType | InterfaceType" >>
        [](auto e) { return e[0].evaluate(); };
    g["IgnoredType"] << "'circle' | '()' | 'diamond' | '<>'";

    // body of containers
    g["ElementBody"]
            << "((Method | Variable | Enumerator | Ignored | WARN_Unrecognized_Line)? WARN_Extepted_EOL? Endl)*" >>
        [this](auto e) { return evaluateBody(e); };

    // simple containers
    g["ElementDef"] << "ElementType Name (Color | Stereotype)? (OpenBrackets ElementBody CloseBrackets)?" >>
        [this](auto e) {
            Element elem{toNamespace(e["Name"]),
                         toName(e["Stereotype"]),
                         {},
                         {},
                         std::get<ElementType>(e["ElementType"]->evaluate().element)};
            SyntaxNode n{elem};
            if (e["ElementBody"]) {
                n.children = std::move(e["ElementBody"]->evaluate().children);
            }
            n.children.push_back(SyntaxNode{End{EndType::Element}});
            return n;
        };
    g["IgnoredDef"] << "IgnoredType Name (Color | Stereotype)?" >> [](auto /*e*/) { return SyntaxNode{std::string()}; };

    // collector rule
    g["Element"] << "ElementDef | IgnoredDef";

    // ========= PACKAGE =========
    g["Package"] << "'package' Name (Color | Stereotype)? OpenBrackets Body CloseBrackets" >> [this](auto e) {
        auto n     = SyntaxNode{Container{toNamespace(e["Name"]), "", ContainerType::Package}};
        n.children = std::move(e["Body"]->evaluate().children);
        n.children.emplace_back(End{EndType::Container});
        return n;
    };

    // ========= NAMESPACE =========
    g["Namespace"] << "'namespace' Name Color? OpenBrackets Body CloseBrackets" >> [this](auto e) {
        auto n     = SyntaxNode{Container{toNamespace(e["Name"]), "", ContainerType::Namespace}};
        n.children = std::move(e["Body"]->evaluate().children);
        n.children.emplace_back(End{EndType::Container});
        return n;
    };

    // ========= DIAGRAM =========
    g["Body"] << "((Element | Relationship | ExternalMethod | ExternalVariable | Package | Namespace | Setter | "
                 "Ignored | WARN_Unrecognized_Line)? WARN_Extepted_EOL? Endl)*" >>
        [this](auto e) { return evaluateBody(e); };
    g["Start"] << "Endl* '@startuml' Name? Endl" >> [this](auto e) {
        return SyntaxNode{Container{toNamespace(e["Name"]), "", ContainerType::Document}};
    };
    g["End"] << "'@enduml' Endl*" >> [](auto /*e*/) { return SyntaxNode{End{EndType::Container}}; };
    g["Diagram"] << "Start Body End" >> [](auto e) {
        SyntaxNode n = e["Start"]->evaluate();
        n.children   = std::move(e["Body"]->evaluate().children);
        n.children.push_back(e["End"]->evaluate());
        return n;
    };

    g.setStart(g["Diagram"]);
}

bool Parser::parse(std::string_view input)
{
    try {
        newLinePositions.push_back(0);
        for (size_t i = 0; i < input.size(); ++i) {
            if (input[i] == '\n') {
                newLinePositions.push_back(i + 1);
            }
        }
        root = g.run(input);
        return true;
    } catch (peg_parser::InterpreterError& err) {
        std::cout << "caught interpreter error: " << err.what() << std::endl;
    } catch (peg_parser::SyntaxError& err) {
        std::cout << "caught syntax error: " << err.what() << std::endl;
    }

    return false;
}

void Parser::visitAST(AbstractVisitor& visitor)
{
    // depth-first search through the tree
    std::stack<const SyntaxNode*> nodeStack;
    nodeStack.push(&root);
    while (!nodeStack.empty()) {
        const SyntaxNode* top = nodeStack.top();
        nodeStack.pop();

        bool visitChildren = std::visit([&visitor](auto&& arg) -> bool { return visitor.visit(arg); }, top->element);
        if (visitChildren) {
            for (const auto& child : top->children) {
                nodeStack.push(&child);
            }
        }
    }
}

std::string Parser::toName(Expression e)
{
    auto name = removePadding(e.view());
    // remove double quotes
    if (name[0] == '"' && name[name.size() - 1] == '"') {
        name.remove_prefix(1);
        name.remove_suffix(1);
    }
    return std::string(name);
}

std::string Parser::toName(std::optional<Expression> e)
{
    return e ? toName(*e) : "";
}

std::string_view Parser::removePadding(std::string_view in)
{
    // remove leading and trailing spaces
    in.remove_prefix(std::min(in.find_first_not_of(' '), in.size()));
    in.remove_suffix(in.size() - std::min(in.find_last_not_of(' ') + 1, in.size()));
    return in;
}

std::list<std::string> Parser::toNamespace(Expression e)
{
    std::list<std::string> out;
    auto fullName = removePadding(e.view());
    if (fullName[0] == '"' && fullName[fullName.size() - 1] == '"') {
        fullName.remove_prefix(1);
        fullName.remove_suffix(1);
        out.push_back(std::string(fullName));
    } else {
        for (const auto& ns :
             fullName | std::ranges::views::split(namespaceDelimiter) | std::ranges::views::transform([](auto&& rng) {
                 return std::string_view(&*rng.begin(), std::ranges::distance(rng));
             })) {
            out.push_back(std::string(ns));
        }
    }

    return out;
}

std::list<std::string> Parser::toNamespace(std::optional<Expression> e)
{
    if (e) {
        return toNamespace(*e);
    }
    return std::list<std::string>{};
}

SyntaxNode Parser::evaluateBody(const Expression& e)
{
    SyntaxNode ret;
    for (auto expr : e) {
        SyntaxNode n = expr.evaluate();
        if (!std::holds_alternative<std::string>(n.element)) {
            ret.children.push_back(std::move(n));
        }
    }
    return ret;
}

} // namespace PlantUml