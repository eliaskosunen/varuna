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
#include "fe/api/App.h"

#include "llvm/Config/llvm-config.h"

namespace fe
{
	namespace cli
	{
		static const std::string ver = fmt::format("{} (LLVM version: {}) Build date: {}",
				util::programinfo::version::toString(), LLVM_VERSION_STRING, util::programinfo::getBuildDate());

		CLI::CLI(int argc_, char **argv_)
			: argc(argc_), argv(argv_),
			cmd(util::programinfo::name, ' ', ver) {}

		int CLI::run()
		{
			try
			{
				TCLAP::ValueArg<std::string> loggingArg("", "logging",
						"Logging level to use. Valid values: 'trace', 'debug' and 'info' (default).",
						false, "info", "Logging level");
				cmd.add(loggingArg);

				TCLAP::SwitchArg licenseArg("", "license", "Print the license and copyright information", false);
				cmd.add(licenseArg);

				TCLAP::ValueArg<int> threadsArg("j", "jobs", "Number of threads to use. Default: 1", false, 1, "Threads");
				cmd.add(threadsArg);

				TCLAP::UnlabeledMultiArg<std::string> fileArg("file", "Files to process", false, "List of files to process");
				cmd.add(fileArg);

				cmd.parse(argc, argv);

				int threads = threadsArg.getValue();
				auto app = std::make_unique<api::Application>(threads);

				if(!setLoggingLevel(app.get(), loggingArg.getValue()))
				{
					throw TCLAP::ArgException("Unsupported value for --logging, see --help", loggingArg.longID());
				}

				bool lic = licenseArg.getValue();
				if(lic)
				{
					showLicense();
					return 0;
				}

				std::vector<std::string> files = fileArg.getValue();
				if(files.empty())
				{
					throw TCLAP::ArgException("File to process is required, see --help", fileArg.longID());
				}

				bool result = app->execute(std::move(files));
				if(!result)
				{
					util::logger->info("Compilation failed");
				}
				else
				{
					util::logger->info("Compilation successful");
				}

				return result ? 0 : 1;
			}
			catch(TCLAP::ArgException &e)
			{
				util::logger->error("Command line error: {}: {}", e.error(), e.argId());
				return 1;
			}
		}

		bool CLI::setLoggingLevel(api::Application *app, const std::string &level)
		{
			if(level == "trace")
			{
				app->setLoggingLevel(api::Application::LOG_TRACE);
			}
			else if(level == "debug")
			{
				app->setLoggingLevel(api::Application::LOG_DEBUG);
			}
			else if(level == "info")
			{
				app->setLoggingLevel(api::Application::LOG_INFO);
			}
			else
			{
				return false;
			}
			return true;
		}

		void CLI::showLicense() const
		{
			util::loggerBasic->info("Varuna, {}\n", ver);

			util::loggerBasic->info("Varuna is published under the GNU General Public License version 3 (GNU GPL 3, the License)");
			util::loggerBasic->info("Varuna - Copyright (C) 2016 Elias Kosunen");
			util::loggerBasic->info("This program comes with ABSOLUTELY NO WARRANTY; see the License for details.");
			util::loggerBasic->info("This is free software, and you are welcome to redistribute it \
under certain conditions; see the License for details.");
		}
	} // namespace cli
} // namespace fe
