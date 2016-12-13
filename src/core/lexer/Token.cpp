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
			switch(type.get())
			{
			case TOKEN_KEYWORD_IMPORT:
				return "KEYWORD_IMPORT";
			case TOKEN_IDENTIFIER:
				return "IDENTIFIER";

			case TOKEN_LITERAL_INTEGER:
				return "LITERAL_INTEGER";
			case TOKEN_LITERAL_FLOAT:
				return "LITERAL_FLOAT";
			case TOKEN_LITERAL_STRING:
				return "LITERAL_STRING";
			case TOKEN_LITERAL_TRUE:
				return "LITERAL_TRUE";
			case TOKEN_LITERAL_FALSE:
				return "LITERAL_FALSE";
			case TOKEN_LITERAL_NONE:
				return "LITERAL_NONE";

			case TOKEN_OPERATORB_ADD:
				return "OPERATORB_ADD";
			case TOKEN_OPERATORB_MUL:
				return "OPERATORB_MUL";

			case TOKEN_EOF:
				return "EOF";
			case TOKEN_DEFAULT:
				return "DEFAULT";
			default:
				return "[unknown]";
			}
		}
	} // namespace lexer
} // namespace core
