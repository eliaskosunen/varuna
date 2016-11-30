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

/**
 * \file
 * Main file of the application
 */

#include <iostream>
#include <stdexcept>
#include <cstdlib>

#include "fe/cli/CLI.h"
#include "util/Logger.h"

static void cleanup()
{
	util::dropLogger();
}

/**
 * The entry point of the application
 * @param  argc Argument count
 * @param  argv Argument vector
 * @return      Application return value
 */
int main(int argc, char **argv)
{
	try
	{
		const int atexitResult = std::atexit(cleanup);
		if(atexitResult != 0)
		{
			std::cerr << "Failed to register cleanup function\n";
			return -1;
		}

		util::initLogger();

		fe::cli::CLI commandLineInterface(argc, argv);
		return commandLineInterface.run();
	}
	catch(const spdlog::spdlog_ex &e)
	{
		std::cerr << "EXCEPTION: Logging failed: " << e.what() << "\n";
	}
	catch(const std::exception &e)
	{
		std::cerr << "EXCEPTION: " << e.what() << "\n";
	}
	return 1;
}
