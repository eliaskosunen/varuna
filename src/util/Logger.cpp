// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#include "util/Logger.h"
#include "util/ProgramInfo.h"
#include <iostream>
#include <string>

namespace util
{
std::shared_ptr<spdlog::logger> logger =
    spdlog::stdout_color_mt("console_color");
std::shared_ptr<spdlog::logger> loggerBasic =
    spdlog::stdout_logger_mt("console_nocolor");

void initLogger()
{
    static bool set = false;
    if(set)
    {
        return;
    }
#if 0
		spdlog::set_pattern("[%Y-%m-%d %T.%f] [%l] [%n] %v");
		logger->set_pattern("[%Y-%m-%d %T.%f] [%l] %v");
		loggerBasic->set_pattern("%v");
#endif
    spdlog::set_pattern(std::string(programinfo::name) +
                        " [thread %t] [%n] [%l]: %v");
    logger->set_pattern(std::string(programinfo::name) +
                        " [thread %t] [%l]: %v");
    loggerBasic->set_pattern("%v");

    logger->flush_on(spdlog::level::err);
    loggerBasic->flush_on(spdlog::level::err);
}

void dropLogger()
{
    spdlog::drop_all();
}
} // namespace util
