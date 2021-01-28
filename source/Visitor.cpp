#include "Visitor.h"

#include <algorithm>

PlantUML Visitor::getResult() 
{
    return root;
}

void Visitor::visitDiagram(Expression start, Expression body) 
{
    root.type = PlantUML::Type::Diagram;

    start.evaluate(*this);
    body.evaluate(*this);
}

void Visitor::visitContainer(Expression e, PlantUML::Type type) 
{
    currentContainer->subData.emplace_back(type, currentContainer);
    currentContainer = &currentContainer->subData.back();

    for (auto s : e)
        s.evaluate(*this);

    currentContainer = currentContainer->parent;
}

void Visitor::visitOpeningBracket() 
{
    if (!currentContainer->subData.empty())
        currentContainer = &currentContainer->subData.back();
}

void Visitor::visitClosingBracket() 
{
    if (currentContainer->parent)
        currentContainer = currentContainer->parent;
}

void Visitor::visitField(Expression valueType, Expression name) 
{
    currentContainer->subData.emplace_back(PlantUML::Type::Field, currentContainer);
    auto& field = currentContainer->subData.back();

    field.name = name.string();
    field.valueType = valueType.string();
}

void Visitor::visitExternalField(Expression container, Expression field) 
{
    auto containerName = container.string();
    auto containerIt = std::ranges::find(currentContainer->subData, containerName, &PlantUML::name);

    if (containerIt == currentContainer->subData.end()) {
        currentContainer = &currentContainer->subData.emplace_back(PlantUML::Type::Class, currentContainer);
        currentContainer->name = containerName;
    } else {
        currentContainer = &(*containerIt);
    }

    field.evaluate(*this);
    currentContainer = currentContainer->parent;
}

void Visitor::visitMethod(Expression valueType, Expression name, Expression parameters) 
{
    currentContainer->subData.emplace_back(PlantUML::Type::Method, currentContainer);
    currentContainer = &currentContainer->subData.back();
    currentContainer->name = name.string();
    currentContainer->valueType = valueType.string();

    parameters.evaluate(*this);

    currentContainer = currentContainer->parent;
}

void Visitor::visitVoidMethod(Expression name, Expression parameters) 
{
    currentContainer->subData.emplace_back(PlantUML::Type::Method, currentContainer);
    currentContainer = &currentContainer->subData.back();
    currentContainer->name = name.string();
    currentContainer->valueType = "void";

    parameters.evaluate(*this);

    currentContainer = currentContainer->parent;
}

void Visitor::visitExternalMethod(Expression container, Expression method) 
{
    auto containerName = container.string();
    auto containerIt = std::ranges::find(currentContainer->subData, containerName, &PlantUML::name);

    if (containerIt == currentContainer->subData.end()) {
        currentContainer = &currentContainer->subData.emplace_back(PlantUML::Type::Class, currentContainer);
        currentContainer->name = containerName;
    } else {
        currentContainer = &(*containerIt);
    }

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
    auto strName = name.string();
    auto firstQuote = strName.find_first_of('"');
    if (firstQuote < strName.size()) {
        strName.erase(firstQuote,1);
        strName.erase(strName.find_last_of('"'));
    }
    strName.erase(strName.find_last_not_of(' ')+1);
    currentContainer->name = strName;
}
