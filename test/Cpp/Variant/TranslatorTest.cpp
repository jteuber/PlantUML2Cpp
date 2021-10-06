#include "gtest/gtest.h"

#include "Cpp/Variant/Translator.h"
#include "PlantUml/ModelElement.h"
#include "PlantUml/Parser.h"

namespace puml = PlantUml;

namespace Cpp::Variant {

TEST(VariantTranslatorTest, EmptyDiagram)
{
    // Arrange
    Translator sut{std::make_shared<Config>()};

    puml::Container c{{}, "", puml::ContainerType::Document};
    puml::End ec{puml::EndType::Document};

    // Act
    sut.visit(c);
    sut.visit(ec);

    // Assert
    auto variants = std::move(sut).results();
    ASSERT_TRUE(variants.empty());
}

TEST(VariantTranslatorTest, SimpleVariant)
{
    // Arrange
    Translator sut{std::make_shared<Config>()};

    puml::Container c{{}, "", puml::ContainerType::Document};
    puml::Element e{{"Vari"}, "", 'V'};
    puml::End ec{puml::EndType::Document};

    // Act
    sut.visit(c);
    sut.visit(e);
    sut.visit(ec);

    // Assert
    auto variants = std::move(sut).results();
    ASSERT_EQ(variants.size(), 1);
    EXPECT_EQ(variants[0].name, "Vari");
}

TEST(VariantTranslatorTest, VariantWithOneType)
{
    // Arrange
    Translator sut{std::make_shared<Config>()};

    puml::Container c{{}, "", puml::ContainerType::Document};
    puml::Element e{{"Vari"}, "", 'V'};
    puml::Relationship r{{"Vari"}, {"Type"}, "", "", "", false, puml::RelationshipType::Aggregation};
    puml::End ec{puml::EndType::Document};

    // Act
    sut.visit(c);
    sut.visit(e);
    sut.visit(r);
    sut.visit(ec);

    // Assert
    auto variants = std::move(sut).results();
    ASSERT_EQ(variants.size(), 1);
    EXPECT_EQ(variants[0].name, "Vari");
    ASSERT_EQ(variants[0].containedTypes.size(), 1);
    EXPECT_EQ(variants[0].containedTypes[0].base, "Type");
}

TEST(VariantTranslatorTest, VariantWithTwoTypesAndOtherRelationship)
{
    // Arrange
    Translator sut{std::make_shared<Config>()};

    puml::Container c{{}, "", puml::ContainerType::Document};
    puml::Element e{{"Vari"}, "", 'V'};
    puml::Relationship r1{{"Vari"}, {"Type"}, "", "", "", false, puml::RelationshipType::Composition};
    puml::Relationship r2{{"Vari"}, {"OtherType"}, "", "", "", false, puml::RelationshipType::Usage};
    puml::End ec{puml::EndType::Document};

    // Act
    sut.visit(c);
    sut.visit(e);
    sut.visit(r1);
    sut.visit(r2);
    sut.visit(ec);

    // Assert
    auto variants = std::move(sut).results();
    ASSERT_EQ(variants.size(), 1);
    EXPECT_EQ(variants[0].name, "Vari");
    ASSERT_EQ(variants[0].containedTypes.size(), 2);
    EXPECT_EQ(variants[0].containedTypes[0].base, "Type");
    EXPECT_EQ(variants[0].containedTypes[1].base, "OtherType");
}

TEST(VariantTranslatorTest, VariantWithTypesInside)
{
    // Arrange
    Translator sut{std::make_shared<Config>()};

    puml::Container c{{}, "", puml::ContainerType::Document};
    puml::Element e{{"Vari"}, "", 'V'};
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
    auto variants = std::move(sut).results();
    ASSERT_EQ(variants.size(), 1);
    EXPECT_EQ(variants[0].name, "Vari");
    ASSERT_EQ(variants[0].containedTypes.size(), 2);
    EXPECT_EQ(variants[0].containedTypes[0].base, "Type1");
    EXPECT_EQ(variants[0].containedTypes[1].base, "Type2");
}

} // namespace Cpp::Variant