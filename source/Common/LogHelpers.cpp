#include "Common/LogHelpers.h"

#include <spdlog/spdlog.h>

void logFuncEntry(const std::source_location location)
{
    spdlog::trace("Entering: {}", location.function_name());
}

void logFuncExit(const std::source_location location)
{
    spdlog::trace("Exiting: {}", location.function_name());
}