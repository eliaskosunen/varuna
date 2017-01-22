// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#pragma once

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
} // namespace util
