// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#pragma once

#include <spdlog.h>

namespace util
{
/**
 * Colored and stylized logger.
 * @see The spdlog documentation
 */
extern std::shared_ptr<spdlog::logger> logger;
/**
 * Unstylized logger.
 * @see The spdlog documentation
 */
extern std::shared_ptr<spdlog::logger> loggerBasic;

inline auto createLogger(bool isColor, const std::string& name = "Logger")
{
    if(isColor)
    {
        // std::rand() is baaaaaaaaad
        return spdlog::stdout_color_mt(fmt::format("{}-{}", name, std::rand()));
    }
    return spdlog::stdout_logger_mt(fmt::format("{}-{}", name, std::rand()));
}

/**
 * Set logger styles
 */
void initLogger();

void dropLogger();
} // namespace util
