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

namespace core
{
	namespace lexer
	{
		enum TokenType
		{
			TOKEN_DEFAULT,

			TOKEN_UNKNOWN,			/// Unknown
			TOKEN_IDENTIFIER,		/// Variables, functions etc.
			TOKEN_LITERAL,			/// String literals, numbers
			TOKEN_OPERATOR,			/// Operators: + - * / etc.
			TOKEN_KEYWORD,			/// Reserved words: if while function etc.
			TOKEN_CONTROL_OPERATOR	/// Operators without arguments: { } [ ]
		};

		class Token
		{
		protected:
			TokenType type;

		public:
			Token(TokenType _type = TOKEN_DEFAULT);

			void setType(TokenType _type);
			TokenType getType() const;

			std::string toString() const;
		};
	}
}
