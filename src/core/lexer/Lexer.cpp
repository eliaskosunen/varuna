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

#include "core/lexer/Lexer.h"

#include <string>
#include <vector>

#include "core/grammar/Grammar.h"
#include "util/Logger.h"

namespace core
{
	namespace lexer
	{
		TokenVector Lexer::run(const std::string &str, bool &error, const std::string &filename)
		{
			// Initialize variables
			TokenVector vec;
			std::string buffer;
			buffer.reserve(16); // Reserve some space to speed up
			Token currentToken;
			unsigned long currentLine = 1;
			error = false;

			std::string::const_iterator strpointer = str.begin();
			const std::string::const_iterator strend = str.end();

			currentToken.setFile(filename);

			for(; strpointer != strend; ++strpointer)
			{
				const char currentChar = *strpointer;

				util::loggerBasic->trace("");
				util::logger->trace("Current character: '{}', on line: {}", currentChar, currentLine);

				// Increase currentLine if current character is a newline character
				if(currentChar == '\n')
				{
					++currentLine;
				}

				// If whitespace, continue to next non-whitespace character
				if(std::isspace(currentChar))
				{
					char curr = currentChar;
					while(std::isspace(curr))
					{
						++strpointer;
						curr = *strpointer;
					}
					--strpointer;
					continue;
				}
			}

			return vec;
		}
	}
}
