#include "gtest/gtest.h"

#include "Cpp/IncludeGatherer.h"

namespace Cpp {

TEST(IncludeGathererTest, NoPrimitives)
{
    // Arrange
    IncludeGatherer sut{std::make_shared<Config>()};

    Class test{"Test"};
    test.body.emplace_back(Variable{"var1", {"int"}});
    test.body.emplace_back(Variable{"var2", {"float"}});
    test.body.emplace_back(Variable{"var3", {"unsigned int"}});
    test.body.emplace_back(Variable{"var4", {"double"}});
    test.body.emplace_back(Variable{"var4", {"bool"}});

    // Act
    sut.gather(test);

    // Assert
    EXPECT_TRUE(test.localIncludes.empty());
    EXPECT_TRUE(test.externalIncludes.empty());
}

TEST(IncludeGathererTest, StdFromVariable)
{
    // Arrange
    IncludeGatherer sut{std::make_shared<Config>()};

    Class test{"Test"};
    test.body.emplace_back(Variable{"var", "std::string"});

    // Act
    sut.gather(test);

    // Assert
    EXPECT_TRUE(test.localIncludes.empty());
    EXPECT_EQ(test.externalIncludes.size(), 1);
    EXPECT_NE(test.externalIncludes.find("string"), test.externalIncludes.end());
}

TEST(IncludeGathererTest, ExternalFromMethod)
{
    // Arrange
    IncludeGatherer sut{std::make_shared<Config>()};

    Class test{"Test"};
    test.body.emplace_back(Method{"method", "boost::something"});

    // Act
    sut.gather(test);

    // Assert
    EXPECT_TRUE(test.localIncludes.empty());
    EXPECT_EQ(test.externalIncludes.size(), 1);
    EXPECT_NE(test.externalIncludes.find("something.h"), test.externalIncludes.end());
}

} // namespace Cpp
