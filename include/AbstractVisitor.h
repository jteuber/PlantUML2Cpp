#pragma once

#include <peg_parser/generator.h>

class AbstractVisitor;
using Expression = peg_parser::Interpreter<void, AbstractVisitor&>::Expression;

class AbstractVisitor
{
public:
    virtual void visitStart(std::optional<Expression> name) = 0;

    // Containers
    virtual void visitClass(Expression type,
                            Expression name,
                            std::optional<Expression> stereotype) = 0;
    virtual void visitEnum(Expression name)                       = 0;
    virtual void visitPackage(Expression name)                    = 0;
    virtual void visitNamespace(Expression name)                  = 0;
    virtual void visitOpeningBracket()                            = 0;
    virtual void visitClosingBracket()                            = 0;

    // Fields and methods
    virtual void visitField(Expression valueType,
                            Expression name,
                            std::optional<Expression> visibility)           = 0;
    virtual void visitExternalField(Expression container, Expression field) = 0;

    virtual void visitParameter(Expression valueType, Expression name) = 0;
    virtual void visitMethod(Expression name,
                             Expression parameters,
                             std::optional<Expression> returnType,
                             std::optional<Expression> visibility)     = 0;
    virtual void visitExternalMethod(Expression container,
                                     Expression method)                = 0;

    virtual void visitPrivateVisibility()        = 0;
    virtual void visitProtectedVisibility()      = 0;
    virtual void visitPackagePrivateVisibility() = 0;
    virtual void visitPublicVisibility()         = 0;

    // Relationships aka connectors
    virtual void visitRelationship(Expression subject,
                                   Expression connector,
                                   Expression object,
                                   std::optional<Expression> objectCardinality,
                                   std::optional<Expression> subjectCardinality,
                                   std::optional<Expression> label) = 0;
    virtual void visitExtension()                                   = 0;
    virtual void visitComposition()                                 = 0;
    virtual void visitAggregation()                                 = 0;
    virtual void visitUsage()                                       = 0;

    // Options
    virtual void visitSetNamespaceSeparator(Expression separator) = 0;
};