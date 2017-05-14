// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#include "util/Logger.h"
#include "util/MathUtils.h"
#include "util/ProgramInfo.h"
#include "util/StringUtils.h"
#include <string>

namespace util
{
// Create loggers
std::shared_ptr<spdlog::logger> logger = createLogger(true, "console_color");
std::shared_ptr<spdlog::logger> loggerBasic =
    createLogger(false, "console_nocolor");

void initLogger()
{
    static bool set = false;
    if(set)
    {
        return;
    }

    // Set logger patterns
    // Varuna [name] level: message
    spdlog::set_pattern(std::string(programinfo::name) + " [%n] %l: %v");
    // Varuna level: message
    logger->set_pattern(std::string(programinfo::name) + " %l: %v");
    // Message
    loggerBasic->set_pattern("%v");

    // Only flush on errors
    logger->flush_on(spdlog::level::err);
    loggerBasic->flush_on(spdlog::level::err);
}

void dropLogger()
{
    spdlog::drop_all();
}
} // namespace util
