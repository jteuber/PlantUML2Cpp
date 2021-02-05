#pragma once

#include <stack>

#include <peg_parser/generator.h>

#include "PlantUML.h"

class Visitor;
using Expression = peg_parser::Interpreter<void, Visitor &>::Expression;

class Visitor
{
public:
    PlantUMLPtr getResult();

    void visitStart(std::optional<Expression> name);

    void visitContainer(Expression e, PlantUML::Type type);
    void visitOpeningBracket();
    void visitClosingBracket();

    void visitField(Expression valueType, Expression name);
    void visitExternalField(Expression container, Expression field);

    void visitMethod(Expression valueType, Expression name, Expression parameters);
    void visitVoidMethod(Expression name, Expression parameters);
    void visitExternalMethod(Expression container, Expression method);

    void visitRelationship(Expression subject, Expression objectPart, std::optional<Expression> label);
    void visitObject(Expression object);
    void visitCardinality(Expression e);
    void visitRelationshipLabel(Expression e);

    void visitExtension(Expression object);
    void visitComposition(Expression object);
    void visitAggregation(Expression object);
    void visitUsage(Expression object);

    void visitName(Expression name);

private:
    PlantUMLPtr findOrCreateChild(std::string_view name, PlantUML::Type type = PlantUML::Type::Class);
    std::string prepareNameString(Expression name);

private:
    PlantUMLPtr root = std::make_shared<PlantUML>(PlantUML::Type::Diagram, nullptr);
    std::stack<PlantUMLPtr> namespaceStack = std::stack<PlantUMLPtr>{{root}}; // Namespace in which to look for elements
    PlantUMLPtr currentContainer = root;                                      // Container into which to put new containers
    PlantUMLPtr currentElement = root;                                        // Container into which to put new elements
};
