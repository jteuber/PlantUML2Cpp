#include "Common/LogHelpers.h"

#include <spdlog/spdlog.h>

FuncTracer::FuncTracer(const std::source_location location)
    : m_location(location)
{
    spdlog::trace("Entering: {} ({}, line {},{})",
                  m_location.function_name(),
                  m_location.file_name(),
                  m_location.line(),
                  m_location.column());
}

FuncTracer::~FuncTracer()
{
    spdlog::trace("Exiting: {} ({}, line {},{})",
                  m_location.function_name(),
                  m_location.file_name(),
                  m_location.line(),
                  m_location.column());
}