#pragma once

#include <source_location>

class FuncTracer
{
public:
    FuncTracer(const std::source_location location = std::source_location::current());
    ~FuncTracer();

private:
    std::source_location m_location;
};
