/*
Copyright (C) 2016 Elias Kosunen

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "util/Logger.h"

#include <string>
#include <iostream>

#include "spdlog/spdlog.h"

namespace util
{
	std::shared_ptr<spdlog::logger> logger = spdlog::stdout_color_st("console_color");
	std::shared_ptr<spdlog::logger> loggerBasic = spdlog::stdout_logger_st("console_nocolor");

	void initLogger()
	{
		static bool set = false;
		if(set)
		{
			return;
		}
		spdlog::set_pattern("[%Y-%m-%d %T.%f] [%l] [%n] %v");
		logger->set_pattern("[%Y-%m-%d %T.%f] [%l] %v");
		loggerBasic->set_pattern("%v");
	}
}
