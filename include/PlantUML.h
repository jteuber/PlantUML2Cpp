#pragma once

#include <string>
#include <vector>
#include <memory>

#include <algorithm>

struct PlantUML;
using PlantUMLPtr = std::shared_ptr<PlantUML>;

struct PlantUML : std::enable_shared_from_this<PlantUML> 
{
    std::string name;
    std::string valueType; // empty for most, return/var type for methods and fields
    std::string modifier; // stereotypes, annotations
    enum class Type {
        Abstract,
        Annotation,
        Circle,
        Class,
        Diamond,
        Entity,
        Enum,
        Interface,
        Namespace,
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
        : type(type)
        , parent(parent)
    {}

    PlantUML(const std::string& name, Type type, PlantUMLPtr parent) 
        : name(name)
        , type(type)
        , parent(parent)
    {}

    PlantUMLPtr createChild(PlantUML::Type type = PlantUML::Type::Class)
    {
        return subData.emplace_back(std::make_shared<PlantUML>(type, shared_from_this()));
    }

    PlantUMLPtr findOrCreateChild(const std::string& name, PlantUML::Type type = PlantUML::Type::Class)
    {
        auto containerIt = std::ranges::find(subData, name, &PlantUML::name);
        if (containerIt != subData.end()) 
        {
            return *containerIt;
        }
        else 
        {
            return subData.emplace_back(std::make_shared<PlantUML>(name, type, shared_from_this()));
        } 
    }

    PlantUMLPtr findChild(const std::string& name)
    {
        auto containerIt = std::ranges::find(subData, name, &PlantUML::name);
        if (containerIt != subData.end()) 
        {
            return *containerIt;
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