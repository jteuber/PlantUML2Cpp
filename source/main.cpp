#include <filesystem>

#include "PlantUML2Cpp.h"

int main(int argc, char** argv)
{
    std::filesystem::path path = std::filesystem::current_path();
    if (argc >= 2) {
        path = std::filesystem::path(argv[1]);
    }

    PlantUML2Cpp p;
    return p.run(path) ? 0 : -1;
}
