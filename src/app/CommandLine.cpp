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

#include <string>
#include <iostream>

#include "app/CommandLine.h"
#include "util/CommandLineParser.h"
#include "util/StreamReader.h"
#include "core/preprocessor/Preprocessor.h"
#include "core/lexer/Token.h"
#include "core/lexer/Lexer.h"
#include "util/Logger.h"

namespace app
{

	CommandLine::CommandLine(int &argc, char **argv)
	{
		clp = new util::CommandLineParser(argc, argv);
	}

	CommandLine::~CommandLine()
	{
		delete clp;
	}

	void CommandLine::run() const
	{
		if(clp->optionExists("--logging=trace"))
		{
			spdlog::set_level(spdlog::level::trace);
		}
		else if(clp->optionExists("--logging=debug"))
		{
			spdlog::set_level(spdlog::level::debug);
		}
		else if(clp->optionExists("--logging=default") || clp->optionExists("--logging=info"))
		{
			spdlog::set_level(spdlog::level::info);
		}

		if(clp->optionExists("-cp") || clp->optionExists("--copyright") || clp->optionExists("--license"))
		{
			copyright();
			return;
		}

		if(clp->optionExists("-h") || clp->optionExists("-?") || clp->optionExists("--help")) {
			help();
			return;
		}

		std::string file = clp->getOption("-f");
		if(clp->isDefault(file) || file.empty()) {
			util::logger->error("No file given! No '-f' command line parameter found.");
			return;
		}

		util::logger->trace("File to process: '{}'", file);

		util::StreamReader sr;
		std::string filec = sr.readFile(file);
		if(filec.empty() || filec == "ERROR") {
			util::logger->error("Failed to read input file '{}'. Check if the file exists.", file);
			return;
		}

		util::logger->trace("File contents:\n{}\n", filec);

		util::logger->debug("Starting preprocessor.");
		core::preprocessor::Preprocessor prep;
		std::string code = prep.run(filec);
		util::logger->debug("Preprocessing finished.");
		util::logger->trace("Preprocessed file:\n{}\n", code);

		util::logger->debug("Starting lexer.");
		core::lexer::Lexer lexer;
		bool lexerError = false;
		core::lexer::TokenVector tokens = lexer.run(code, lexerError, file);
		if(lexerError)
		{
			return;
		}
		util::logger->debug("Lexing finished.");
		for(const auto &t : tokens)
		{
			util::logger->trace("Token: ({}): {}", t.typeToString(), t.getValue());
		}
	}

	void CommandLine::help() const
	{
		util::loggerBasic->info("Help");
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
	}

}
