#include "ClassBuilder.h"

#include <algorithm>
#include <iostream>
#include <ranges>

ClassBuilder::ClassBuilder(/* args */) {}

ClassBuilder::~ClassBuilder() {}

const std::vector<Class>& ClassBuilder::results()
{
    return m_classes;
}

void ClassBuilder::visitClass(Expression type,
                              Expression name,
                              std::optional<Expression> stereotype)
{
    Class c;

    if (type.view().starts_with("abstract")) {
        c.type = Class::Type::Abstract;
    } else if (type.view() == "class") {
        c.type = Class::Type::Class;
    } else if (type.view() == "entity") {
        c.type = Class::Type::Struct;
    } else if (type.view() == "interface") {
        c.type = Class::Type::Interface;
    }

    c.namespaceStack = m_namespaceStack;

    auto fullName = name.view();
    fullName.remove_prefix(
        std::min(fullName.find_first_not_of(' '), fullName.size()));
    fullName.remove_suffix(
        fullName.size() -
        std::min(fullName.find_last_not_of(' ') + 1, fullName.size()));
    if (fullName[0] == '"' && fullName[fullName.size() - 1] == '"') {
        fullName.remove_prefix(1);
        fullName.remove_suffix(1);
        c.name = fullName;
    } else {
        for (const auto& ns :
             fullName | std::ranges::views::split(namespaceDelimiter) |
                 std::ranges::views::transform([](auto&& rng) {
                     return std::string_view(&*rng.begin(),
                                             std::ranges::distance(rng));
                 })) {
            c.namespaceStack.push(ns);
        }
        c.name = c.namespaceStack.top();
        c.namespaceStack.pop();
    }

    m_classes.push_back(c);
    m_lastEncounteredClass = --m_classes.end();
}

void ClassBuilder::visitNamespace(Expression name) {}

void ClassBuilder::visitOpeningBracket() {}

void ClassBuilder::visitClosingBracket() {}

void ClassBuilder::visitField(Expression valueType,
                              Expression name,
                              std::optional<Expression> visibility)
{
    Variable var;
    auto& c = *m_lastEncounteredClass;

    var.name   = name.view();
    var.type   = valueType.view();
    var.source = Relationship::Member;

    if (visibility) {
        visibility->evaluate(*this);
        var.visibility = m_lastEncounteredVisibility;
    } else if (c.type == Class::Type::Interface) {
        std::cout << "ERROR: Interface with variable encountered" << std::endl;
    }

    c.variables.push_back(var);
}

void ClassBuilder::visitExternalField(Expression container, Expression field)
{
    // m_lastEncounteredClass = *std::ranges::find(m_classes, container.view(),
    // &Class::name);
}

void ClassBuilder::visitParameter(Expression valueType, Expression name)
{
    Parameter param;
    param.name = name.view();
    param.type = valueType.view();
    m_lastEncounteredClass->methods.back().parameters.push_back(param);
}

void ClassBuilder::visitMethod(Expression name,
                               Expression parameters,
                               std::optional<Expression> returnType,
                               std::optional<Expression> visibility)
{
    Method method;
    auto& c = *m_lastEncounteredClass;

    method.name       = name.view();
    method.returnType = returnType ? returnType->view() : "void";

    if (visibility) {
        visibility->evaluate(*this);
        method.visibility = m_lastEncounteredVisibility;
    } else if (c.type == Class::Type::Struct) {
        std::cout << "ERROR: Struct with method encountered" << std::endl;
    }

    c.methods.push_back(method);

    parameters.evaluate(*this);
}

void ClassBuilder::visitExternalMethod(Expression container, Expression method)
{}

void ClassBuilder::visitPrivateVisibility()
{
    m_lastEncounteredVisibility = Visibility::Private;
}

void ClassBuilder::visitProtectedVisibility()
{
    m_lastEncounteredVisibility = Visibility::Protected;
}

void ClassBuilder::visitPackagePrivateVisibility()
{
    m_lastEncounteredVisibility = Visibility::PackagePrivate;
}

void ClassBuilder::visitPublicVisibility()
{
    m_lastEncounteredVisibility = Visibility::Public;
}

void ClassBuilder::visitRelationship(
    Expression subject,
    Expression connector,
    Expression object,
    std::optional<Expression> objectCardinality,
    std::optional<Expression> subjectCardinality,
    std::optional<Expression> label)
{
    connector.evaluate(*this);

    m_lastEncounteredClass =
        std::ranges::find(m_classes, prepareNameString(subject), &Class::name);

    if (m_lastEncounteredClass != m_classes.end()) {
        switch (m_lastRelationship) {
        case Relationship::Extension:
            m_lastEncounteredClass->parents.push_back(
                prepareNameString(object));
            break;

        case Relationship::Composition:
        case Relationship::Aggregation: {
            Variable var;
            var.type   = prepareNameString(object);
            var.source = m_lastRelationship;
            if (objectCardinality)
                var.cardinality = objectCardinality->view();
            if (label)
                var.name = label->view();
            m_lastEncounteredClass->variables.push_back(var);
            break;
        }
        default:
            break;
        }
    }
}

void ClassBuilder::visitExtension()
{
    m_lastRelationship = Relationship::Extension;
}

void ClassBuilder::visitComposition()
{
    m_lastRelationship = Relationship::Composition;
}

void ClassBuilder::visitAggregation()
{
    m_lastRelationship = Relationship::Aggregation;
}

void ClassBuilder::visitUsage()
{
    m_lastRelationship = Relationship::Usage;
}

void ClassBuilder::visitSetNamespaceSeparator(Expression separator)
{
    namespaceDelimiter = separator.string();
}

std::string_view ClassBuilder::prepareNameString(Expression e)
{
    auto name = e.view();
    // remove leading and trailing spaces
    name.remove_prefix(std::min(name.find_first_not_of(' '), name.size()));
    name.remove_suffix(name.size() -
                       std::min(name.find_last_not_of(' ') + 1, name.size()));
    // remove double quotes
    if (name[0] == '"' && name[name.size() - 1] == '"') {
        name.remove_prefix(1);
        name.remove_suffix(1);
    }
    return name;
}