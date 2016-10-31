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

#include "core/Run.h"

#include <string>

#include "util/Logger.h"
#include "util/StreamReader.h"
#include "core/preprocessor/Preprocessor.h"
#include "core/lexer/Token.h"
#include "core/lexer/Lexer.h"

namespace core
{
	int run(const std::string &filename)
	{
		util::logger->trace("File to process: '{}'", filename);

		std::string filec;
		util::StreamReader sr;
		filec = sr.readFile(filename);
		if(filec.empty() || filec == "ERROR") {
			util::logger->error("Failed to read input file '{}'. Check if the file exists", filename);
			return 1;
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
		core::lexer::TokenVector tokens = lexer.run(code, lexerError, filename);
		if(lexerError)
		{
			return 1;
		}
		util::logger->debug("Lexing finished.");
		for(const auto &t : tokens)
		{
			util::logger->trace("Token: ({}: {}): {}", t.categoryToString(), t.typeToString(), t.getValue());
		}
		return 0;
	}
}
