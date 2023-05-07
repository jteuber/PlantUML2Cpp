

#include "Config.h"
#include "PlantUML2Cpp.h"

int main(int argc, char** argv)
{
    auto config = std::make_shared<Config>();
    if (!config->parseAndLoad(argc, argv)) {
        return -1;
    }

    PlantUML2Cpp puml2cpp(config);
    return puml2cpp.run() ? 0 : -2;
}
