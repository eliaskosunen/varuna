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
		if(clp->optionExists("-h") || clp->optionExists("-?") || clp->optionExists("--help")) {
			help();
			return;
		}

		std::string file = clp->getOption("-f");
		if(clp->isDefault(file)) {
			std::cout << "No file given\n";
			return;
		}

		std::cout << "File: " << file << "\n";

		util::StreamReader sr;
		std::string filec = sr.readFile(file);
		if(filec.empty() || filec == "ERROR") {
			std::cerr << "Error reading file\n";
			return;
		}

		std::cout << "File contents:\n" << filec << "\n";

		std::cout << "Preprocessing\n";
		core::preprocessor::Preprocessor prep;
		prep.run(filec);
		std::cout << "Preprocessed file:\n" << filec << "\n\n";

		std::cout << "Lexer output:\n";

		core::lexer::Lexer lexer;
		core::lexer::TokenVector tokens = lexer.run(filec);
		for(const auto &t : tokens)
		{
			std::cout << "Token(" << t.typeToString() << "): " << t.getValue() << "\n";
		}
	}

	void CommandLine::help() const
	{
		std::cout << "help\n";
	}

}
