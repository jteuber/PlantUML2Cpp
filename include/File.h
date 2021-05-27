#pragma once

#include <filesystem>
#include <string>

struct File
{
    std::filesystem::path path;
    std::string content;
};
