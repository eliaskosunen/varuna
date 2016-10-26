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

#include "core/lexer/Token.h"
#include "util/StringUtils.h"

namespace core
{
	namespace lexer
	{
		TokenType Lexer::identifyBuffer(const std::string &buf)
		{
			return TOKEN_DEFAULT;
		}

		TokenVector Lexer::run(const std::string &str)
		{
			TokenVector tokens;
			std::string buffer;
			buffer.reserve(8);
			std::string::const_iterator strpointer = str.begin();
			std::string::const_iterator strendpointer = str.end();

			for(; strpointer != strendpointer; ++strpointer)
			{
				Token t;
				//t.setType(identifyBuffer(buffer));
				t.setType(TOKEN_DEFAULT);
				t.setValue(util::StringUtils::charToString(*strpointer));
				tokens.push_back(t);
			}

			return tokens;
		}
	}
}
