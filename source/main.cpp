#include <iostream>
#include <fstream>
#include <array>

#include "Parser.h"

std::string readFullFile(std::string filename)
{
    std::string input;
    std::ifstream file(filename);
    for (std::array<char, 256> buffer; file.getline(&buffer[0], 256); ) 
    {
        input.append(buffer.data());
        input.append("\n");
    }

    return input;
}

static const char * const TYPE2STRING[] =  {"Abstract",
            "Annotation",
            "Circle",
            "Class",
            "Diamond",
            "Entity",
            "Enum",
            "Interface"};

int main(int argc, char** argv) 
{
    if (argc != 2)
    {
        std::cout << "Usage: PlantUML2Cpp <filename>" << std::endl;
        return 1;
    }

    // read the input file
    std::string filename(argv[1]);
    std::string input = readFullFile(filename);

    std::cout << input << std::endl;

    // parse the input
    Parser parser;
    PlantUML result = parser.parse(input);

    for (auto& element : result.elements)
    {
        std::cout << TYPE2STRING[static_cast<int>(element.type)] << " " << element.name << std::endl;
    }
}
