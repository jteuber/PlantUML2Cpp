#pragma once

#include <string>
#include <vector>
#include <map>

struct PlantUML {
    std::string name;
    std::string valueType; // empty for most, return/var type for methods and fields
    std::string modifier; // stereotypes
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
        Empty
    } type;
    
    std::vector<PlantUML> subData;
    PlantUML* parent = nullptr;

    PlantUML() 
        : type(Type::Empty)
    {}

    PlantUML(Type type, PlantUML* parent) 
        : type(type)
        , parent(parent)
    {}

    PlantUML(const std::string& name, Type type, PlantUML* parent) 
        : name(name)
        , type(type)
        , parent(parent)
    {}
};