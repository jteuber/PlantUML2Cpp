#pragma once

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "AbstractVisitor.h"

struct VisitorMock : public AbstractVisitor
{
    MOCK_METHOD(void, visitStart, (std::optional<Expression> name));

    // Containers
    MOCK_METHOD(void, visitClass, (Expression type, Expression name, std::optional<Expression> stereotype));
    MOCK_METHOD(void, visitEnum, (Expression name));
    MOCK_METHOD(void, visitPackage, (Expression name));
    MOCK_METHOD(void, visitNamespace, (Expression name));
    MOCK_METHOD(void, visitOpeningBracket, ());
    MOCK_METHOD(void, visitClosingBracket, ());

    // Fields and methods
    MOCK_METHOD(void, visitField, (Expression valueType, Expression name, std::optional<Expression> visibility));
    MOCK_METHOD(void, visitExternalField, (Expression container, Expression field));

    MOCK_METHOD(void, visitMethod, (Expression name, Expression parameters, std::optional<Expression> returnType, std::optional<Expression> visibility));
    MOCK_METHOD(void, visitExternalMethod, (Expression container, Expression method));

    MOCK_METHOD(void, visitPrivateVisibility, ());
    MOCK_METHOD(void, visitProtectedVisibility, ());
    MOCK_METHOD(void, visitPackagePrivateVisibility, ());
    MOCK_METHOD(void, visitPublicVisibility, ());

    // Relationships aka connectors
    MOCK_METHOD(void, visitRelationship, (Expression subject, Expression connector, Expression object, std::optional<Expression> objectCardinality, std::optional<Expression> subjectCardinality, std::optional<Expression> label));
    MOCK_METHOD(void, visitExtension, ());
    MOCK_METHOD(void, visitComposition, ());
    MOCK_METHOD(void, visitAggregation, ());
    MOCK_METHOD(void, visitUsage, ());

    // Options
    MOCK_METHOD(void, visitSetNamespaceSeparator, (Expression separator));
};