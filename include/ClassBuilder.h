#pragma once

#include "NoopVisitor.h"
#include "Class.h"

class ClassBuilder : public NoopVisitor
{
public:
    ClassBuilder(/* args */);
    ~ClassBuilder();

    const std::vector<Class>& results();

    // Containers
    void visitClass(Expression type, Expression name, std::optional<Expression> stereotype) override;
    void visitEnum(Expression name) override;
    void visitPackage(Expression name) override;
    void visitNamespace(Expression name) override;
    void visitOpeningBracket() override;
    void visitClosingBracket() override;

    // Fields and methods
    void visitField(Expression valueType, Expression name, std::optional<Expression> visibility) override;
    void visitExternalField(Expression container, Expression field) override;

    void visitMethod(Expression name, Expression parameters, std::optional<Expression> returnType, std::optional<Expression> visibility) override;
    void visitExternalMethod(Expression container, Expression method) override;

    void visitPrivateVisibility() override;
    void visitProtectedVisibility() override;
    void visitPackagePrivateVisibility() override;
    void visitPublicVisibility() override;

    // Relationships aka connectors
    void visitRelationship(Expression subject, Expression connector, Expression object, std::optional<Expression> objectCardinality, std::optional<Expression> subjectCardinality, std::optional<Expression> label) override;
    void visitExtension() override;
    void visitComposition() override;
    void visitAggregation() override;
    void visitUsage() override;

    // Options
    void visitSetNamespaceSeparator(Expression separator) override;

private:
    std::vector<Class> m_classes;
    std::stack<std::string_view> m_namespaceStack;

    std::string namespaceDelimiter = ".";
};