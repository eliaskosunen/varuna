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

#pragma once

#include <string>
#include <vector>

#include "core/lexer/Token.h"

namespace core
{
	namespace lexer
	{
		typedef std::vector<core::lexer::Token> TokenVector;

		class Lexer
		{
			const std::string &content;
			const std::string &filename;

			unsigned int currentLine;

			TokenType getTokenTypeFromWord(const std::string &buf) const;
			Token getTokenFromWord(const std::string &buf) const;
			Token createToken(TokenType type, const std::string &val) const;

			Token getNextToken(std::string::const_iterator &it);
		public:
			Lexer(const std::string &cont, const std::string &file) : content(cont), filename(file), currentLine(1) {}

			TokenVector run(bool &error);
		};
	}
}
