#include "gtest/gtest.h"

#include "Cpp/Class/MemberSorter.h"

namespace Cpp {
namespace Class {

TEST(MemberSorterTest, NoPrimitives)
{
    // Arrange
    MemberSorter sut;

    Class test{"Test"};
    test.body.emplace_back(Variable{"var1", {"int"}});
    test.body.emplace_back(Variable{"var2", {"float"}});
    test.body.emplace_back(Method{"method1", {"unsigned int"}});
    test.body.emplace_back(Variable{"var3", {"double"}});
    test.body.emplace_back(Method{"method2", {"bool"}});

    // Act
    sut.sort(test);

    // Assert
    EXPECT_EQ(test.body.size(), 8);
    EXPECT_EQ(std::get<VisibilityKeyword>(test.body[2]).name, "public:");
    EXPECT_EQ(std::get<VisibilityKeyword>(test.body[4]).name, "private:");
    EXPECT_EQ(std::get<VisibilityKeyword>(test.body[6]).name, "public:");
}

} // namespace Class
} // namespace Cpp
