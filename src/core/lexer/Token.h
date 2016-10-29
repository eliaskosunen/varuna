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
#include <map>

namespace core
{
	namespace lexer
	{
		enum TokenCategory
		{
			TOKEN_CAT_DEFAULT,
			TOKEN_CAT_UNKNOWN,

			TOKEN_CAT_WORD,
			TOKEN_CAT_LITERAL,
			TOKEN_CAT_OPERATOR
		};

		enum TokenType
		{
			TOKEN_DEFAULT,
			TOKEN_UNKNOWN,

			TOKEN_WORD_DEFAULT,
			TOKEN_WORD_KEYWORD,
			TOKEN_WORD_IDENTIFIER,

			TOKEN_LITERAL_DEFAULT,
			TOKEN_LITERAL_DEFAULT_NUMBER,
			TOKEN_LITERAL_STRING,
			TOKEN_LITERAL_INTEGER,
			TOKEN_LITERAL_FLOAT,
			TOKEN_LITERAL_CHAR,

			TOKEN_OPERATOR
		};

		class Token
		{
		protected:
			TokenCategory cat;
			TokenType type;

			std::string value;

			TokenType getTypeFromCategory(TokenCategory _cat) const;
			TokenCategory getCategoryFromType(TokenType _type) const;

		public:
			const static std::string DEFAULT_PARAM;

			Token() : Token(TOKEN_DEFAULT) {}
			Token(TokenCategory _cat);
			Token(TokenType _type);

			void setCategory(TokenCategory _cat);
			TokenCategory getCategory() const;

			void setType(TokenType _type);
			TokenType getType() const;

			void setValue(std::string val);
			const std::string &getValue() const;

			std::string categoryToString() const;
			std::string typeToString() const;

			void reset();
		};
	}
}
