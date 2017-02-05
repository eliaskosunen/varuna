// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#pragma once

#include "util/File.h"
#include "util/SourceLocation.h"
#include <spdlog.h>

namespace util
{
extern std::shared_ptr<spdlog::logger> logger;
extern std::shared_ptr<spdlog::logger> loggerBasic;

/// Create a new logger
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

std::string formatCompilerMessage(File* file, SourceLocation loc);

template <typename... Args>
inline std::nullptr_t logCompilerError(File* file, SourceLocation loc,
                                       const std::string& format,
                                       Args&&... args)
{
    assert(logger);
    logger->error("{}: {}\n{}", loc.toString(),
                  fmt::format(format, std::forward<Args>(args)...),
                  formatCompilerMessage(file, loc));
    return nullptr;
}

template <typename... Args>
inline void logCompilerWarning(File* file, SourceLocation loc,
                               const std::string& format, Args&&... args)
{
    assert(logger);
    logger->warn("{}: {}\n{}", loc.toString(),
                 fmt::format(format, std::forward<Args>(args)...),
                 formatCompilerMessage(file, loc));
}

template <typename... Args>
inline void logCompilerInfo(File* file, SourceLocation loc,
                            const std::string& format, Args&&... args)
{
    assert(logger);
    logger->info("{}: {}\n{}", loc.toString(),
                 fmt::format(format, std::forward<Args>(args)...),
                 formatCompilerMessage(file, loc));
}
} // namespace util
