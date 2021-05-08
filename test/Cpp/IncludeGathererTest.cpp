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

TEST(IncludeGathererTest, MultipleFromVariable)
{
    // Arrange
    IncludeGatherer sut{std::make_shared<Config>()};

    Class test{"Test"};
    test.body.emplace_back(Variable{"var", "std::string"});
    test.body.emplace_back(Variable{"vec", "std::vector"});
    test.body.emplace_back(Variable{"vec", "Test"});

    // Act
    sut.gather(test);

    // Assert
    EXPECT_EQ(test.localIncludes.size(), 1);
    EXPECT_NE(test.localIncludes.find("Test.h"), test.localIncludes.end());

    EXPECT_EQ(test.externalIncludes.size(), 2);
    EXPECT_NE(test.externalIncludes.find("string"), test.externalIncludes.end());
    EXPECT_NE(test.externalIncludes.find("vector"), test.externalIncludes.end());
}

} // namespace Cpp
