#pragma once

#include "Class.h"
#include "NoopVisitor.h"

class ClassBuilder : public NoopVisitor
{
public:
    ClassBuilder(/* args */);
    ~ClassBuilder();

    const std::vector<Class>& results();

    // Containers
    void visitClass(Expression type,
                    Expression name,
                    std::optional<Expression> stereotype) override;
    void visitNamespace(Expression name) override;
    void visitOpeningBracket() override;
    void visitClosingBracket() override;

    // Fields and methods
    void visitField(Expression valueType,
                    Expression name,
                    std::optional<Expression> visibility) override;
    void visitExternalField(Expression container, Expression field) override;

    void visitParameter(Expression valueType, Expression name) override;
    void visitMethod(Expression name,
                     Expression parameters,
                     std::optional<Expression> returnType,
                     std::optional<Expression> visibility) override;
    void visitExternalMethod(Expression container, Expression method) override;

    void visitPrivateVisibility() override;
    void visitProtectedVisibility() override;
    void visitPackagePrivateVisibility() override;
    void visitPublicVisibility() override;

    // Relationships aka connectors
    void visitRelationship(Expression subject,
                           Expression connector,
                           Expression object,
                           std::optional<Expression> objectCardinality,
                           std::optional<Expression> subjectCardinality,
                           std::optional<Expression> label) override;
    void visitExtension() override;
    void visitComposition() override;
    void visitAggregation() override;
    void visitUsage() override;

    // Options
    void visitSetNamespaceSeparator(Expression separator) override;

private:
    std::string_view prepareNameString(Expression e);

private:
    std::vector<Class> m_classes;
    std::stack<std::string_view> m_namespaceStack;
    Visibility m_lastEncounteredVisibility;
    Relationship m_lastRelationship;
    std::vector<Class>::iterator m_lastEncounteredClass;

    std::string namespaceDelimiter = ".";
};
