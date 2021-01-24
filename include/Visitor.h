#pragma once

#include <peg_parser/generator.h>

#include "PlantUML.h"

class Visitor;
using Expression = peg_parser::Interpreter<void, Visitor &>::Expression;

class Visitor
{
public:
    PlantUML getResult();

    void visitDiagram(Expression start, Expression body);
    void visitContainer(Expression e, PlantUML::Type type);
    void visitOpeningBracket();
    void visitClosingBracket();
    void visitField(Expression valueType, Expression name);
    void visitExternalField(Expression container, Expression field);
    void visitMethod(Expression valueType, Expression name, Expression parameters);
    void visitVoidMethod(Expression name, Expression parameters);
    void visitExternalMethod(Expression container, Expression method);
    void visitName(Expression name);

private:
    PlantUML root;
    PlantUML* currentContainer = &root;
};
