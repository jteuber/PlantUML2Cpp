#pragma once

#include <string>
#include <vector>
#include <map>

struct EnumValue {
    std::string name;
};

struct Field {
    std::string name;
    std::string type;
};

struct Method {
    std::string name;
    std::string returnType;
    std::vector<Field> parameters;
};

struct Container {
    enum class Type {
        Abstract,
        Annotation,
        Circle,
        Class,
        Diamond,
        Entity,
        Enum,
        Interface
    } type;
    std::string name;

    std::vector<EnumValue> enumValues;
    std::vector<Field> fields;
    std::vector<Method> methods;
};

struct PlantUML {
    std::string name;
    
    std::vector<Container> containers;
    std::vector<PlantUML> namespaces;
};