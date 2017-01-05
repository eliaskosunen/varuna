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

/**
 * Set logger styles
 */
void initLogger();

void dropLogger();
} // namespace util
