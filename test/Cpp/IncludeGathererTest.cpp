#include "gtest/gtest.h"

#include "Cpp/IncludeGatherer.h"

namespace Cpp {

TEST(IncludeGathererTest, StdFromVariable)
{
    // Arrange
    IncludeGatherer sut{std::make_shared<Config>()};

    Class test{"Test"};
    test.body.emplace_back(Variable{"var", "std::string"});

    // Act
    sut.gather(test);

    // Assert
    EXPECT_EQ(test.includes.size(), 1);
    EXPECT_NE(test.includes.find("string"), test.includes.end());
}

} // namespace Cpp
