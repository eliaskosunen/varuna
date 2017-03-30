// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#pragma once

#include "util/File.h"
#include "util/SourceLocation.h"
#include <guid.h>
#include <spdlog.h>

namespace util
{
/// Global colored logger
extern std::shared_ptr<spdlog::logger> logger;
/// Global basic logger
extern std::shared_ptr<spdlog::logger> loggerBasic;

/**
 * Create a new logger
 * @param isColor Does the logger have colors
 * @param name    Name of the logger
 */
inline auto createLogger(bool isColor, const std::string& name = "Logger")
{
    if(isColor)
    {
        return spdlog::stdout_color_mt(
            fmt::format("{}-{}", name, GuidGenerator{}.newGuid()));
    }
    return spdlog::stdout_logger_mt(
        fmt::format("{}-{}", name, GuidGenerator{}.newGuid()));
}

/// Initialize logger and loggerBasic
void initLogger();

/// Drop all loggers
void dropLogger();

/**
 * Log a compiler error
 * @param  loc    Location of error
 * @param  format Format string of the message
 * @param  args   Format arguments
 * @return        nullptr
 */
template <typename... Args>
inline std::nullptr_t logCompilerError(const SourceLocation& loc,
                                       const std::string& format,
                                       Args&&... args)
{
    assert(logger);
    logger->error("In {}:\n{}\n{}", loc.toString(),
                  fmt::format(format, std::forward<Args>(args)...),
                  loc.getErrorMessage());
    return nullptr;
}

/**
 * Log a compiler warning
 * @param  loc    Location of warning
 * @param  format Format string of the message
 * @param  args   Format arguments
 */
template <typename... Args>
inline void logCompilerWarning(const SourceLocation& loc,
                               const std::string& format, Args&&... args)
{
    assert(logger);
    logger->warn("In {}:\n{}\n{}", loc.toString(),
                 fmt::format(format, std::forward<Args>(args)...),
                 loc.getErrorMessage());
}

/**
 * Log a compiler notice
 * @param  loc    Location of notice
 * @param  format Format string of the message
 * @param  args   Format arguments
 */
template <typename... Args>
inline void logCompilerInfo(const SourceLocation& loc,
                            const std::string& format, Args&&... args)
{
    assert(logger);
    logger->info("In {}:\n{}\n{}", loc.toString(),
                 fmt::format(format, std::forward<Args>(args)...),
                 loc.getErrorMessage());
}
} // namespace util
