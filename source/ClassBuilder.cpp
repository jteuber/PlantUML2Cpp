#include "ClassBuilder.h"

#include <algorithm>
#include <iostream>
#include <ranges>

const std::vector<Class>& ClassBuilder::results()
{
    return m_classes;
}

void ClassBuilder::visitStereotype(std::optional<Expression> identifier)
{
    if (identifier && m_lastEncounteredClass != m_classes.end()) {
        m_lastEncounteredClass->stereotype = prepareNameString(*identifier);
    }
}

void ClassBuilder::visitClass(Expression type,
                              Expression name,
                              std::optional<Expression> stereotype,
                              std::optional<Expression> body)
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
    splitNamespacedName(name.view(), c.namespaceStack);
    c.name = c.namespaceStack.back();
    c.namespaceStack.pop_back();

    m_classes.push_back(c);
    m_lastEncounteredClass = --m_classes.end();

    if (stereotype) {
        stereotype->evaluate(*this);
    }
    if (body) {
        body->evaluate(*this);
    }
    m_lastEncounteredClass = m_classes.end();
}

void ClassBuilder::visitNamespace(Expression name, Expression body)
{
    auto size = m_namespaceStack.size();
    splitNamespacedName(name.view(), m_namespaceStack);
    body.evaluate(*this);
    while (m_namespaceStack.size() > size) {
        m_namespaceStack.pop_back();
    }
}

void ClassBuilder::visitField(Expression valueType, Expression name, std::optional<Expression> visibility)
{
    if (m_lastEncounteredClass != m_classes.end()) {
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
}

void ClassBuilder::visitExternalField(Expression container, Expression field)
{
    m_lastEncounteredClass = std::ranges::find(m_classes, container.view(), &Class::name);
    field.evaluate(*this);
    m_lastEncounteredClass = m_classes.end();
}

void ClassBuilder::visitParameter(Expression valueType, Expression name)
{
    if (m_lastEncounteredClass != m_classes.end()) {
        Parameter param;
        param.name = name.view();
        param.type = valueType.view();
        m_lastEncounteredClass->methods.back().parameters.push_back(param);
    }
}

void ClassBuilder::visitMethod(Expression name,
                               Expression parameters,
                               std::optional<Expression> returnType,
                               std::optional<Expression> visibility)
{
    if (m_lastEncounteredClass != m_classes.end()) {
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
}

void ClassBuilder::visitExternalMethod(Expression container, Expression method)
{
    m_lastEncounteredClass = std::ranges::find(m_classes, container.view(), &Class::name);
    method.evaluate(*this);
    m_lastEncounteredClass = m_classes.end();
}

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

void ClassBuilder::visitRelationship(Expression subject,
                                     Expression connector,
                                     Expression object,
                                     std::optional<Expression> objectCardinality,
                                     std::optional<Expression> /*subjectCardinality*/,
                                     std::optional<Expression> label)
{
    connector.evaluate(*this);

    m_lastEncounteredClass = std::ranges::find(m_classes, prepareNameString(subject), &Class::name);

    if (m_lastEncounteredClass != m_classes.end()) {
        switch (m_lastRelationship) {
        case Relationship::Extension:
            m_lastEncounteredClass->parents.push_back(std::string(prepareNameString(object)));
            break;

        case Relationship::Composition:
        case Relationship::Aggregation: {
            Variable var;
            var.type   = prepareNameString(object);
            var.source = m_lastRelationship;
            if (objectCardinality) {
                var.cardinality = prepareNameString(*objectCardinality);
            }
            if (label) {
                var.name = prepareNameString(*label);
            }
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
    auto name = removePadding(e.view());
    // remove double quotes
    if (name[0] == '"' && name[name.size() - 1] == '"') {
        name.remove_prefix(1);
        name.remove_suffix(1);
    }
    return name;
}

std::string_view ClassBuilder::removePadding(std::string_view in)
{
    // remove leading and trailing spaces
    in.remove_prefix(std::min(in.find_first_not_of(' '), in.size()));
    in.remove_suffix(in.size() - std::min(in.find_last_not_of(' ') + 1, in.size()));
    return in;
}

void ClassBuilder::splitNamespacedName(std::string_view name, std::list<std::string>& out)
{
    auto fullName = removePadding(name);
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
}
