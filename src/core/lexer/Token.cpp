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

#include "core/lexer/Token.h"

#include <string>

namespace core
{
	namespace lexer
	{
		std::string Token::typeToString() const
		{
			switch(type)
			{
			case TOKEN_KEYWORD_IMPORT:
				return "KEYWORD_IMPORT";
			case TOKEN_IDENTIFIER:
				return "IDENTIFIER";
			case TOKEN_LITERAL_NUMBER:
				return "LITERAL_NUMBER";
			case TOKEN_LITERAL_STRING:
				return "LITERAL_STRING";
			case TOKEN_EOF:
				return "EOF";
			case TOKEN_DEFAULT:
				return "DEFAULT";
			default:
				return "[unknown]";
			}
		}
	}
}
