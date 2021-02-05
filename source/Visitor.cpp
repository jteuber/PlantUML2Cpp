#include "Visitor.h"

#include <algorithm>

PlantUMLPtr Visitor::getResult()
{
    return root;
}

void Visitor::visitStart(Expression e)
{
    if (e.size() > 1)
        root->name = e[1].string();
}

void Visitor::visitContainer(Expression e, PlantUML::Type type)
{
    currentContainer = currentContainer->createChild(type);

    for (auto s : e)
        s.evaluate(*this);

    currentContainer = currentContainer->parent;
}

void Visitor::visitOpeningBracket()
{
    if (currentContainer->getLastChild())
    {
        currentContainer = currentContainer->getLastChild();
        if (currentContainer->type == PlantUML::Type::Namespace)
            namespaceStack.push(currentContainer);
    }
}

void Visitor::visitClosingBracket()
{
    if (currentContainer->parent)
    {
        if (currentContainer->type == PlantUML::Type::Namespace)
            namespaceStack.pop();
        currentContainer = currentContainer->parent;
    }
}

void Visitor::visitField(Expression valueType, Expression name)
{
    auto field = findOrCreateChild(name.string(), PlantUML::Type::Field);
    field->valueType = valueType.string();
}

void Visitor::visitExternalField(Expression container, Expression field)
{
    currentContainer = findOrCreateChild(container.string());
    field.evaluate(*this);
    currentContainer = currentContainer->parent;
}

void Visitor::visitMethod(Expression valueType, Expression name, Expression parameters)
{
    currentContainer = findOrCreateChild(name.string(), PlantUML::Type::Method);
    currentContainer->valueType = valueType.string();

    parameters.evaluate(*this);

    currentContainer = currentContainer->parent;
}

void Visitor::visitVoidMethod(Expression name, Expression parameters)
{
    currentContainer = findOrCreateChild(name.string(), PlantUML::Type::Method);
    currentContainer->valueType = "void";

    parameters.evaluate(*this);

    currentContainer = currentContainer->parent;
}

void Visitor::visitExternalMethod(Expression container, Expression method)
{
    currentContainer = findOrCreateChild(container.string());
    method.evaluate(*this);
    currentContainer = currentContainer->parent;
}

void Visitor::visitRelationship(Expression subject, Expression objectPart, std::optional<Expression> label)
{
    currentElement = findOrCreateChild(subject.string());
    objectPart.evaluate(*this);
    if (label)
        label->evaluate(*this);
}

void Visitor::visitObject(Expression object)
{
    findOrCreateChild(object.string());
    currentElement->valueType = object.string();
}

void Visitor::visitCardinality(Expression e)
{
    currentElement->modifier = prepareNameString(e);
}

void Visitor::visitRelationshipLabel(Expression e)
{
    // TODO: this is ugly! But it's not possible to get the relationship label in scope of composition and aggregation. Or is it?
    currentElement->subData.back()->name = prepareNameString(e);
}

void Visitor::visitExtension(Expression object)
{
    object.evaluate(*this);
}

void Visitor::visitComposition(Expression object)
{
    currentElement = currentElement->createChild(PlantUML::Type::Composition);
    object.evaluate(*this);
    currentElement = currentElement->parent;
}

void Visitor::visitAggregation(Expression object)
{
    currentElement = currentElement->createChild(PlantUML::Type::Aggregation);
    object.evaluate(*this);
    currentElement = currentElement->parent;
}

void Visitor::visitUsage(Expression object)
{
    object.evaluate(*this);
}

void Visitor::visitName(Expression name)
{
    currentContainer->name = prepareNameString(name);
}

PlantUMLPtr Visitor::findOrCreateChild(std::string_view name, PlantUML::Type type)
{
    auto child = namespaceStack.top()->findChild(name);
    if (child != nullptr)
    {
        return child;
    }
    else
    {
        return currentContainer->createChild(name, type);
    }
}

std::string Visitor::prepareNameString(Expression name)
{
    auto strName = name.string();
    auto firstQuote = strName.find_first_of('"');
    if (firstQuote < strName.size())
    {
        strName.erase(firstQuote, 1);
        strName.erase(strName.find_last_of('"'));
    }
    strName.erase(strName.find_last_not_of(' ') + 1);
    return strName;
}
