#pragma once

#include <peg_parser/generator.h>

#include "PlantUML.h"

class Visitor;
using Expression = peg_parser::Interpreter<void, Visitor &>::Expression;

class Visitor
{
public:
    PlantUMLPtr getResult();

    void visitStart(Expression e);

    void visitContainer(Expression e, PlantUML::Type type);
    void visitOpeningBracket();
    void visitClosingBracket();

    void visitField(Expression valueType, Expression name);
    void visitExternalField(Expression container, Expression field);

    void visitMethod(Expression valueType, Expression name, Expression parameters);
    void visitVoidMethod(Expression name, Expression parameters);
    void visitExternalMethod(Expression container, Expression method);

    void visitExtension(Expression parent, Expression child);
    void visitComposition(Expression owner, Expression type);
    void visitAggregation(Expression user, Expression type);
    void visitUsage(Expression client, Expression server);

    void visitName(Expression name);

private:
    std::string prepareNameString(Expression name);

private:
    PlantUMLPtr root = std::make_shared<PlantUML>(PlantUML::Type::Diagram, nullptr);
    PlantUMLPtr currentContainer = root;
};
