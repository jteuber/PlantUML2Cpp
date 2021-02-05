#pragma once

#include <algorithm>
#include <memory>
#include <ranges>
#include <string>
#include <vector>

struct PlantUML;
using PlantUMLPtr = std::shared_ptr<PlantUML>;

struct PlantUML : std::enable_shared_from_this<PlantUML>
{
    std::string name;
    std::string valueType; // empty for most, return/var type for methods and fields
    std::string modifier;  // stereotypes, annotations
    enum class Type
    {
        Abstract,
        Annotation,
        Circle,
        Class,
        Diamond,
        Entity,
        Enum,
        Interface,
        Namespace,
        Package,
        Diagram,
        Method,
        Field,
        EnumValue,
        Composition,
        Aggregation
    } type;

    std::vector<PlantUMLPtr> subData;
    PlantUMLPtr parent = nullptr;

    PlantUML(Type type, PlantUMLPtr parent)
        : type(type), parent(parent)
    {
    }

    PlantUML(std::string_view name, Type type, PlantUMLPtr parent)
        : name(name), type(type), parent(parent)
    {
    }

    PlantUMLPtr createChild(PlantUML::Type type = PlantUML::Type::Class)
    {
        return subData.emplace_back(std::make_shared<PlantUML>(type, shared_from_this()));
    }

    PlantUMLPtr createChild(std::string_view name, PlantUML::Type type = PlantUML::Type::Class)
    {
        return subData.emplace_back(std::make_shared<PlantUML>(name, type, shared_from_this()));
    }

    PlantUMLPtr findChild(std::string_view name)
    {
        auto containerIt = std::ranges::find(subData, name, &PlantUML::name);
        if (containerIt != subData.end())
        {
            return *containerIt;
        }

        auto isPackage = [](PlantUMLPtr p) -> bool { return p->type == PlantUML::Type::Package; };
        for (PlantUMLPtr package : subData | std::views::filter(isPackage))
        {
            auto child = package->findChild(name);
            if (child != nullptr)
            {
                return child;
            }
        }

        return nullptr;
    }

    PlantUMLPtr getLastChild()
    {
        if (!subData.empty())
            return subData.back();
        return nullptr;
    }
};