#include "gtest/gtest.h"

#include "Cpp/Enum/Translator.h"
#include "PlantUml/ModelElement.h"
#include "PlantUml/Parser.h"

namespace puml = PlantUml;

namespace Cpp::Enum {

TEST(EnumTranslatorTest, EmptyDiagram)
{
    // Arrange
    Translator sut{std::make_shared<Config>()};

    puml::Container c{{}, "", puml::ContainerType::Document};
    puml::End ec{puml::EndType::Document};

    // Act
    sut.visit(c);
    sut.visit(ec);

    // Assert
    auto enums = std::move(sut).results();
    ASSERT_TRUE(enums.empty());
}

TEST(EnumTranslatorTest, SimpleEmptyEnum)
{
    // Arrange
    Translator sut{std::make_shared<Config>()};

    puml::Container c{{}, "", puml::ContainerType::Document};
    puml::Element e{{"Enum"}};
    e.type = puml::ElementType::Enum;
    puml::End ec{puml::EndType::Document};

    // Act
    sut.visit(c);
    sut.visit(e);
    sut.visit(ec);

    // Assert
    auto enums = std::move(sut).results();
    ASSERT_EQ(enums.size(), 1);
    EXPECT_EQ(enums[0].name, "Enum");
}

TEST(EnumTranslatorTest, EnumWithContent)
{
    // Arrange
    Translator sut{std::make_shared<Config>()};

    puml::Container c{{}, "", puml::ContainerType::Document};
    puml::Element e{{"Enum"}};
    e.type = puml::ElementType::Enum;
    puml::Enumerator t1{"Type1"};
    puml::Enumerator t2{"Type2"};
    puml::End ee{puml::EndType::Element};
    puml::End ec{puml::EndType::Document};

    // Act
    sut.visit(c);
    sut.visit(e);
    sut.visit(t1);
    sut.visit(t2);
    sut.visit(ee);
    sut.visit(ec);

    // Assert
    auto enums = std::move(sut).results();
    ASSERT_EQ(enums.size(), 1);
    EXPECT_EQ(enums[0].name, "Enum");
    ASSERT_EQ(enums[0].enumerators.size(), 2);
    EXPECT_EQ(enums[0].enumerators[0].name, "Type1");
    EXPECT_EQ(enums[0].enumerators[1].name, "Type2");
}

} // namespace Cpp::Enum
