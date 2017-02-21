// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#pragma once

#include "util/File.h"
#include "util/SourceLocation.h"
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
        // std::rand() is baaaaaaaaad
        return spdlog::stdout_color_mt(fmt::format("{}-{}", name, std::rand()));
    }
    return spdlog::stdout_logger_mt(fmt::format("{}-{}", name, std::rand()));
}

/// Initialize logger and loggerBasic
void initLogger();

/// Drop all loggers
void dropLogger();

/**
 * Format the bottom part of a compiler error message.
 * Format:
 *      | previous row
 *  row | row content
 *      |   ^--- (arrow to the error)
 * @param  loc  Location of error
 * @return      Formatted message
 */
std::string formatCompilerMessage(SourceLocation loc);

/**
 * Log a compiler error
 * @param  loc    Location of error
 * @param  format Format string of the message
 * @param  args   Format arguments
 * @return        nullptr
 */
template <typename... Args>
inline std::nullptr_t
logCompilerError(SourceLocation loc, const std::string& format, Args&&... args)
{
    assert(logger);
    logger->error("In {}:\n{}\n{}", loc.toString(),
                  fmt::format(format, std::forward<Args>(args)...),
                  formatCompilerMessage(loc));
    return nullptr;
}

/**
 * Log a compiler warning
 * @param  loc    Location of warning
 * @param  format Format string of the message
 * @param  args   Format arguments
 */
template <typename... Args>
inline void logCompilerWarning(SourceLocation loc, const std::string& format,
                               Args&&... args)
{
    assert(logger);
    logger->warn("In {}:\n{}\n{}", loc.toString(),
                 fmt::format(format, std::forward<Args>(args)...),
                 formatCompilerMessage(loc));
}

/**
 * Log a compiler notice
 * @param  loc    Location of notice
 * @param  format Format string of the message
 * @param  args   Format arguments
 */
template <typename... Args>
inline void logCompilerInfo(SourceLocation loc, const std::string& format,
                            Args&&... args)
{
    assert(logger);
    logger->info("In {}:\n{}\n{}", loc.toString(),
                 fmt::format(format, std::forward<Args>(args)...),
                 formatCompilerMessage(loc));
}
} // namespace util
