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

#include "app/CommandLine.h"

#include <string>
#include <iostream>
#include <vector>

#include "tclap.h"

#include "util/Logger.h"
#include "core/Run.h"

namespace app
{
	CommandLine::CommandLine(const int &_argc, char **_argv)
		: argc(_argc), argv(_argv), cmd("Varuna", ' ', "0.1")
	{

	}

	int CommandLine::run()
	{
		try
		{
			TCLAP::ValueArg<std::string> loggingArg("l", "logging", "Logging level to use. Valid values: 'trace', 'debug' and 'info' (default).", false, "info", "Logging level");
			cmd.add(loggingArg);

			TCLAP::SwitchArg licenseArg("c", "copyright", "Print the license and copyright information", false);
			cmd.add(licenseArg);

			TCLAP::UnlabeledMultiArg<std::string> fileArg("file", "Files to process", false, "List of files to process");
			cmd.add(fileArg);

			cmd.parse(argc, argv);

			bool license = licenseArg.getValue();
			if(license)
			{
				copyright();
				return 0;
			}

			std::string logging = loggingArg.getValue();
			if(logging == "trace")
			{
				spdlog::set_level(spdlog::level::trace);
			}
			else if(logging == "debug")
			{
				spdlog::set_level(spdlog::level::debug);
			}
			else if(logging == "info")
			{
				spdlog::set_level(spdlog::level::info);
			}
			else
			{
				throw TCLAP::ArgException("Unsupported value for --logging, see --help", loggingArg.longID());
			}

			std::vector<std::string> files = fileArg.getValue();
			if(files.size() == 0)
			{
				throw TCLAP::ArgException("File to process is required, see --help", fileArg.longID());
			}

			return core::run(files[0]);
		}
		catch(TCLAP::ArgException &e)
		{
			util::logger->error("Command line error: {}: {}", e.error(), e.argId());
			return 1;
		}
	}

	void CommandLine::copyright() const
	{
		util::loggerBasic->info("Varuna is published under the GNU General Public License version 3 (GNU GPL 3, the License)");
		util::loggerBasic->info("Varuna - Copyright (C) 2016 Elias Kosunen");
		util::loggerBasic->info("This program comes with ABSOLUTELY NO WARRANTY; see the License for details.");
		util::loggerBasic->info("This is free software, and you are welcome to redistribute it under certain conditions; see the License for details.");
		util::loggerBasic->info("\nVaruna uses the following third-party software. To see the full license details, see the file LICENSE.thirdparty.txt");
		util::loggerBasic->info("Spdlog - Copyright (c) 2016 Gabi Melman. Licensed under the MIT License");
		util::loggerBasic->info("Catch - Licensed under the Boost Software License");
		util::loggerBasic->info("TCLAP - Copyright (c) 2003 Michael E. Smoot. Licensed under the MIT License");
	}

}
