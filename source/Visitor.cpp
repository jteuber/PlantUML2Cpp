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
        currentContainer = currentContainer->getLastChild();
}

void Visitor::visitClosingBracket() 
{
    if (currentContainer->parent)
        currentContainer = currentContainer->parent;
}

void Visitor::visitField(Expression valueType, Expression name) 
{
    auto field = currentContainer->findOrCreateChild(name.string(), PlantUML::Type::Field);
    field->valueType = valueType.string();
}

void Visitor::visitExternalField(Expression container, Expression field) 
{
    currentContainer = currentContainer->findOrCreateChild(container.string());
    field.evaluate(*this);
    currentContainer = currentContainer->parent;
}

void Visitor::visitMethod(Expression valueType, Expression name, Expression parameters) 
{
    currentContainer = currentContainer->findOrCreateChild(name.string(), PlantUML::Type::Method);
    currentContainer->valueType = valueType.string();

    parameters.evaluate(*this);

    currentContainer = currentContainer->parent;
}

void Visitor::visitVoidMethod(Expression name, Expression parameters) 
{
    currentContainer = currentContainer->findOrCreateChild(name.string(), PlantUML::Type::Method);
    currentContainer->valueType = "void";

    parameters.evaluate(*this);

    currentContainer = currentContainer->parent;
}

void Visitor::visitExternalMethod(Expression container, Expression method) 
{
    currentContainer = currentContainer->findOrCreateChild(container.string());
    method.evaluate(*this);
    currentContainer = currentContainer->parent;
}

void Visitor::visitExtension(Expression parent, Expression child) 
{
    currentContainer->findOrCreateChild(parent[0].string());
    auto childClass = currentContainer->findOrCreateChild(child[0].string());

    childClass->valueType = parent[0].string();
}

void Visitor::visitComposition(Expression owner, Expression type) 
{
    auto ownerClass = currentContainer->findOrCreateChild(owner[0].string());
    currentContainer->findOrCreateChild(type[0].string());

    auto field = ownerClass->createChild(PlantUML::Type::Composition);
    field->valueType = type[0].string();
}

void Visitor::visitAggregation(Expression user, Expression type) 
{
    auto userClass = currentContainer->findOrCreateChild(user[0].string());
    currentContainer->findOrCreateChild(type[0].string());

    auto field = userClass->createChild(PlantUML::Type::Aggregation);
    field->valueType = type[0].string();
}

void Visitor::visitUsage(Expression client, Expression server) 
{
    
}

void Visitor::visitName(Expression name) 
{
    currentContainer->name = prepareNameString(name);
}

std::string Visitor::prepareNameString(Expression name) 
{
    auto strName = name.string();
    auto firstQuote = strName.find_first_of('"');
    if (firstQuote < strName.size()) {
        strName.erase(firstQuote,1);
        strName.erase(strName.find_last_of('"'));
    }
    strName.erase(strName.find_last_not_of(' ')+1);
    return strName;
}
