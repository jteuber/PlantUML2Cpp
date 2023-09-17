#pragma once

#include <source_location>

void logFuncEntry(const std::source_location location = std::source_location::current());
void logFuncExit(const std::source_location location = std::source_location::current());
