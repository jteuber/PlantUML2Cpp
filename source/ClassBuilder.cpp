#include "ClassBuilder.h"

#include <ranges>
#include <algorithm>

ClassBuilder::ClassBuilder(/* args */)
{
}

ClassBuilder::~ClassBuilder()
{
}

const std::vector<Class>& ClassBuilder::results() 
{
    return m_classes;
}

void ClassBuilder::visitClass(Expression type, Expression name, std::optional<Expression> stereotype) 
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
    fullName.remove_prefix(std::min(fullName.find_first_not_of(' '), fullName.size()));
    fullName.remove_suffix(fullName.size() - std::min(fullName.find_last_not_of(' ') + 1, fullName.size()));
    if (fullName[0] == '"' && fullName[fullName.size() - 1] == '"')
    {
        fullName.remove_prefix(1);
        fullName.remove_suffix(1);
        c.name = fullName;
    }
    else
    {
        for (const auto& ns : fullName 
                | std::ranges::views::split(namespaceDelimiter) 
                | std::ranges::views::transform([](auto &&rng) {
                        return std::string_view(&*rng.begin(), std::ranges::distance(rng)); }))
        {
            c.namespaceStack.push(ns);
        }
        c.name = c.namespaceStack.top();
        c.namespaceStack.pop();
    }

    m_classes.push_back(c);
}

void ClassBuilder::visitEnum(Expression name) 
{
    
}

void ClassBuilder::visitPackage(Expression name) 
{
    
}

void ClassBuilder::visitNamespace(Expression name) 
{
    
}

void ClassBuilder::visitOpeningBracket() 
{
    
}

void ClassBuilder::visitClosingBracket() 
{
    
}

void ClassBuilder::visitField(Expression valueType, Expression name, std::optional<Expression> visibility) 
{
    
}

void ClassBuilder::visitExternalField(Expression container, Expression field) 
{
    
}

void ClassBuilder::visitMethod(Expression name, Expression parameters, std::optional<Expression> returnType, std::optional<Expression> visibility) 
{
    
}

void ClassBuilder::visitExternalMethod(Expression container, Expression method) 
{
    
}

void ClassBuilder::visitPrivateVisibility() 
{
    
}

void ClassBuilder::visitProtectedVisibility() 
{
    
}

void ClassBuilder::visitPackagePrivateVisibility() 
{
    
}

void ClassBuilder::visitPublicVisibility() 
{
    
}

void ClassBuilder::visitRelationship(Expression subject, Expression connector, Expression object, std::optional<Expression> objectCardinality, std::optional<Expression> subjectCardinality, std::optional<Expression> label) 
{
    
}

void ClassBuilder::visitExtension() 
{
    
}

void ClassBuilder::visitComposition() 
{
    
}

void ClassBuilder::visitAggregation() 
{
    
}

void ClassBuilder::visitUsage() 
{
    
}

void ClassBuilder::visitSetNamespaceSeparator(Expression separator) 
{
    namespaceDelimiter = separator.string();
}