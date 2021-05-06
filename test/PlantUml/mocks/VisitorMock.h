#pragma once

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "PlantUml/AbstractVisitor.h"

using ::testing::An;
using ::testing::Return;

namespace PlantUml {

struct VisitorMock : public AbstractVisitor
{
    VisitorMock()
        : AbstractVisitor()
    {
        ON_CALL(*this, visit(An<const Variable&>())).WillByDefault(Return(true));
        ON_CALL(*this, visit(An<const Method&>())).WillByDefault(Return(true));
        ON_CALL(*this, visit(An<const Relationship&>())).WillByDefault(Return(true));
        ON_CALL(*this, visit(An<const Container&>())).WillByDefault(Return(true));
        ON_CALL(*this, visit(An<const Element&>())).WillByDefault(Return(true));
        ON_CALL(*this, visit(An<const Note&>())).WillByDefault(Return(true));
        ON_CALL(*this, visit(An<const Separator&>())).WillByDefault(Return(true));
        ON_CALL(*this, visit(An<const Enumerator&>())).WillByDefault(Return(true));
        ON_CALL(*this, visit(An<const Type&>())).WillByDefault(Return(true));
        ON_CALL(*this, visit(An<const Parameter&>())).WillByDefault(Return(true));
        ON_CALL(*this, visit(An<const End&>())).WillByDefault(Return(true));
    }

    MOCK_METHOD(bool, visit, (const Variable& v));
    MOCK_METHOD(bool, visit, (const Method& m));
    MOCK_METHOD(bool, visit, (const Relationship& r));
    MOCK_METHOD(bool, visit, (const Container& c));
    MOCK_METHOD(bool, visit, (const Element& e));
    MOCK_METHOD(bool, visit, (const Note& n));
    MOCK_METHOD(bool, visit, (const Separator& s));
    MOCK_METHOD(bool, visit, (const Enumerator& e));
    MOCK_METHOD(bool, visit, (const Type& t));
    MOCK_METHOD(bool, visit, (const Parameter& p));
    MOCK_METHOD(bool, visit, (const End& e));
};

} // namespace PlantUml
