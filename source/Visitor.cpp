#include "Visitor.h"

#include <algorithm>
#include <assert.h>
#include <sstream>

PlantUMLPtr Visitor::getResult()
{
    return root;
}

void Visitor::visitStart(std::optional<Expression> name)
{
    if (name)
        root->name = name->string();
}

void Visitor::visitContainer(Expression name, std::optional<Expression> stereotype, PlantUML::Type type)
{
    currentContainer = findOrCreateInCurrentContext(name, type)->parent;
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
    auto field = findOrCreateInCurrentContext(name, PlantUML::Type::Field);
    field->valueType = valueType.string();
}

void Visitor::visitExternalField(Expression container, Expression field)
{
    currentContainer = findOrCreateInCurrentContext(container);
    field.evaluate(*this);
    currentContainer = currentContainer->parent;
}

void Visitor::visitMethod(Expression valueType, Expression name, Expression parameters)
{
    currentContainer = findOrCreateInCurrentContext(name, PlantUML::Type::Method);
    currentContainer->valueType = valueType.string();

    parameters.evaluate(*this);

    currentContainer = currentContainer->parent;
}

void Visitor::visitVoidMethod(Expression name, Expression parameters)
{
    currentContainer = findOrCreateInCurrentContext(name, PlantUML::Type::Method);
    currentContainer->valueType = "void";

    parameters.evaluate(*this);

    currentContainer = currentContainer->parent;
}

void Visitor::visitExternalMethod(Expression container, Expression method)
{
    currentContainer = findOrCreateInCurrentContext(container);
    method.evaluate(*this);
    currentContainer = currentContainer->parent;
}

void Visitor::visitRelationship(Expression subject, Expression objectPart, std::optional<Expression> label)
{
    currentElement = findOrCreateInCurrentContext(subject);
    objectPart.evaluate(*this);
    if (label)
        label->evaluate(*this);
}

void Visitor::visitObject(Expression object)
{
    findOrCreateInCurrentContext(object);
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

void Visitor::visitSetNamespaceSeparator(Expression separator)
{
    namespaceDelimiter = separator.string();
}

PlantUMLPtr Visitor::findOrCreateInCurrentContext(Expression identifier, PlantUML::Type type)
{
    // first, split the identifier up into namespace parts
    auto [namespaceName, name] = splitNamespacedName(identifier);

    // just a regular name
    if (namespaceName.empty())
    {
        return findOrCreateChild(namespaceStack.top(), currentContainer, name, type);
    }
    // identifier starts with a dot => global namespace (this assumes that the name doesn't contain any additional delimiter characters)
    else if (namespaceName == namespaceDelimiter)
    {
        return findOrCreateChild(root, name, type);
    }
    // it's a namespace
    else
    {
        namespaceName.remove_suffix(namespaceDelimiter.size());
        auto current = root;
        for (auto part : {namespaceName, name})
        {
            current = findOrCreateChild(current, part, PlantUML::Type::Namespace);
        }
        current->type = type;

        return current;
    }
}

PlantUMLPtr Visitor::findOrCreateChild(PlantUMLPtr searchContext, PlantUMLPtr createContext, std::string_view name, PlantUML::Type type)
{
    auto child = searchContext->findChild(name);
    if (child != nullptr)
    {
        return child;
    }
    else
    {
        return createContext->createChild(name, type);
    }
}

PlantUMLPtr Visitor::findOrCreateChild(PlantUMLPtr searchAndCreateContext, std::string_view name, PlantUML::Type type)
{
    return findOrCreateChild(searchAndCreateContext, searchAndCreateContext, name, type);
}

std::string_view Visitor::prepareNameString(Expression e)
{
    auto name = e.view();
    // remove leading and trailing spaces
    name.remove_prefix(std::min(name.find_first_not_of(' '), name.size()));
    name.remove_suffix(name.size() - std::min(name.find_last_not_of(' ') + 1, name.size()));
    // remove double quotes
    if (name[0] == '"' && name[name.size() - 1] == '"')
    {
        name.remove_prefix(1);
        name.remove_suffix(1);
    }

    return name;
}

std::pair<std::string_view, std::string_view> Visitor::splitNamespacedName(Expression e)
{
    auto fullName = e.view();
    // remove leading and trailing spaces
    fullName.remove_prefix(std::min(fullName.find_first_not_of(' '), fullName.size()));
    fullName.remove_suffix(fullName.size() - std::min(fullName.find_last_not_of(' ') + 1, fullName.size()));
    if (fullName[0] == '"' && fullName[fullName.size() - 1] == '"')
    {
        fullName.remove_prefix(1);
        fullName.remove_suffix(1);
        return {"", fullName};
    }

    auto delimiter = std::min(fullName.find_last_of(namespaceDelimiter), fullName.size());
    delimiter = delimiter < fullName.size() ? delimiter + 1 : 0;
    auto namespaceName = fullName.substr(0, delimiter);
    auto name = fullName.substr(delimiter, fullName.size());

    return {namespaceName, name};
}
