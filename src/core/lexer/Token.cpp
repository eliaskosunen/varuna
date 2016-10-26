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
		Token::Token(TokenType _type) : type(_type) {}

		void Token::setType(TokenType _type)
		{
			type = _type;
		}

		TokenType Token::getType() const
		{
			return type;
		}

		std::string Token::toString() const
		{
			switch(type)
			{
			case TOKEN_DEFAULT:
				return "DEFAULT";
			case TOKEN_UNKNOWN:
				return "UNKNOWN";
			case TOKEN_IDENTIFIER:
				return "IDENTIFIER";
			case TOKEN_LITERAL:
				return "LITERAL";
			case TOKEN_OPERATOR:
				return "OPERATOR";
			case TOKEN_KEYWORD:
				return "KEYWORD";
			case TOKEN_CONTROL_OPERATOR:
				return "CONTROL_OPERATOR";
			}
			return "DEFAULT";
		}
	}
}
