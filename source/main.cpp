#include <filesystem>

#include "CLI/App.hpp"
#include "CLI/Formatter.hpp"
#include "CLI/Config.hpp"

#include "PlantUML2Cpp.h"

int main(int argc, char** argv)
{
    CLI::App app{"App description"};

    std::filesystem::path path = std::filesystem::current_path();
    std::string pathString = path.string();
    app.add_option("path", pathString, "Path of project directory containing the models in a folder called models");

    CLI11_PARSE(app, argc, argv);

    path = pathString;

    PlantUML2Cpp p;
    return p.run(path) ? 0 : -1;
}
