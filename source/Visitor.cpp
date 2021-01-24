#include "Visitor.h"
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
