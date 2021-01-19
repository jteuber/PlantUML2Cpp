#pragma once

#include <string>
#include <vector>

struct PlantUML {
    struct Element {
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
    };

    std::vector<Element> elements;
};