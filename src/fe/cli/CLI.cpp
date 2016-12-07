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

#include "fe/cli/CLI.h"
#include "fe/api/API.h"

namespace fe
{
	namespace cli
	{
		int CLI::run()
		{
			try
			{
				TCLAP::ValueArg<std::string> loggingArg("", "logging", "Logging level to use. Valid values: 'trace', 'debug' and 'info' (default).", false, "info", "Logging level");
				cmd.add(loggingArg);

				TCLAP::SwitchArg licenseArg("", "license", "Print the license and copyright information", false);
				cmd.add(licenseArg);

				TCLAP::UnlabeledMultiArg<std::string> fileArg("file", "Files to process", false, "List of files to process");
				cmd.add(fileArg);

				cmd.parse(argc, argv);

				bool lic = licenseArg.getValue();
				if(lic)
				{
					showLicense();
					return 0;
				}

				if(!setLoggingLevel(loggingArg.getValue()))
				{
					throw TCLAP::ArgException("Unsupported value for --logging, see --help", loggingArg.longID());
				}

				std::vector<std::string> files = fileArg.getValue();
				if(files.size() == 0)
				{
					throw TCLAP::ArgException("File to process is required, see --help", fileArg.longID());
				}

				for(const auto &file : files)
				{
					fe::api::addFile(file);
				}
				for(const auto &file : files)
				{
					if(!fe::api::runFile(file)) return 1;
				}
				return 0;
			}
			catch(TCLAP::ArgException &e)
			{
				util::logger->error("Command line error: {}: {}", e.error(), e.argId());
				return 1;
			}
		}

		bool CLI::setLoggingLevel(const std::string &level)
		{
			if(level == "trace")
			{
				fe::api::setLoggingLevel(fe::api::LOG_TRACE);
			}
			else if(level == "debug")
			{
				fe::api::setLoggingLevel(fe::api::LOG_DEBUG);
			}
			else if(level == "info")
			{
				fe::api::setLoggingLevel(fe::api::LOG_INFO);
			}
			else
			{
				return false;
			}
			return true;
		}

		void CLI::showLicense() const
		{
			util::loggerBasic->info("Varuna is published under the GNU General Public License version 3 (GNU GPL 3, the License)");
			util::loggerBasic->info("Varuna - Copyright (C) 2016 Elias Kosunen");
			util::loggerBasic->info("This program comes with ABSOLUTELY NO WARRANTY; see the License for details.");
			util::loggerBasic->info("This is free software, and you are welcome to redistribute it under certain conditions; see the License for details.");
			util::loggerBasic->info("\nVaruna uses the following third-party software. To see the full license details, see the file LICENSE.thirdparty.txt");
			util::loggerBasic->info("Spdlog - Copyright (c) 2016 Gabi Melman. Licensed under the MIT License");
			util::loggerBasic->info("Doctest - Copyright (c) 2016 Viktor Kirilov. Licensed under the MIT License");
			util::loggerBasic->info("TCLAP - Copyright (c) 2003 Michael E. Smoot. Licensed under the MIT License");
			util::loggerBasic->info("Boost Variant - Copyright (c) 2002, 2003 Eric Friedman, Itay Maman. Licensed under the Boost Software License, Version 1.0");
		}
	}
}
