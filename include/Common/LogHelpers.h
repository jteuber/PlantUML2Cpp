#pragma once

#include <source_location>

class FuncTracer
{
public:
    explicit FuncTracer(std::source_location location = std::source_location::current());
    ~FuncTracer();

    // copying or moving not allowed
    FuncTracer(const FuncTracer& other)            = delete;
    FuncTracer operator=(const FuncTracer& other)  = delete;
    FuncTracer(const FuncTracer&& other)           = delete;
    FuncTracer operator=(const FuncTracer&& other) = delete;

private:
    std::source_location m_location;
};
