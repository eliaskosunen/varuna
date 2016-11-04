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
		TokenType Token::getTypeFromCategory(TokenCategory _cat) const
		{
			switch(_cat)
			{
			case TOKEN_CAT_DEFAULT:
				return TOKEN_DEFAULT;
			case TOKEN_CAT_UNKNOWN:
				return TOKEN_UNKNOWN;
			case TOKEN_CAT_WORD:
				return TOKEN_WORD_DEFAULT;
			case TOKEN_CAT_LITERAL:
				return TOKEN_LITERAL_DEFAULT;
			case TOKEN_CAT_OPERATOR:
				return TOKEN_OPERATOR;
			}
			return TOKEN_UNKNOWN;
		}

		TokenCategory Token::getCategoryFromType(TokenType _type) const
		{
			switch (_type) {
			case TOKEN_DEFAULT:
				return TOKEN_CAT_DEFAULT;
			case TOKEN_UNKNOWN:
				return TOKEN_CAT_UNKNOWN;
			case TOKEN_WORD_DEFAULT:
			case TOKEN_WORD_KEYWORD:
			case TOKEN_WORD_IDENTIFIER:
				return TOKEN_CAT_WORD;
			case TOKEN_LITERAL_DEFAULT:
			case TOKEN_LITERAL_DEFAULT_NUMBER:
			case TOKEN_LITERAL_STRING:
			case TOKEN_LITERAL_INTEGER:
			case TOKEN_LITERAL_FLOAT:
			case TOKEN_LITERAL_CHAR:
			case TOKEN_LITERAL_BOOLEAN:
			case TOKEN_LITERAL_NONE:
				return TOKEN_CAT_LITERAL;
			case TOKEN_OPERATOR:
				return TOKEN_CAT_OPERATOR;
			}
			return TOKEN_CAT_UNKNOWN;
		}

		Token::Token(TokenCategory _cat) : value("")
		{
			cat = _cat;
			type = getTypeFromCategory(_cat);
		}
		Token::Token(TokenType _type) : value("")
		{
			type = _type;
			cat = getCategoryFromType(_type);
		}

		void Token::setCategory(TokenCategory _cat)
		{
			cat = _cat;
		}
		TokenCategory Token::getCategory() const
		{
			return cat;
		}

		void Token::setType(TokenType _type)
		{
			type = _type;
		}

		TokenType Token::getType() const
		{
			return type;
		}

		std::string Token::categoryToString() const
		{
			switch(cat)
			{
			case TOKEN_CAT_DEFAULT:
				return "DEFAULT";
			case TOKEN_CAT_UNKNOWN:
				return "UNKNOWN";

			case TOKEN_CAT_WORD:
				return "WORD";
			case TOKEN_CAT_LITERAL:
				return "LITERAL";
			case TOKEN_CAT_OPERATOR:
				return "OPERATOR";
			}
			return "DEFAULT";
		}

		std::string Token::typeToString() const
		{
			switch(type)
			{
			case TOKEN_DEFAULT:
				return "DEFAULT";
			case TOKEN_UNKNOWN:
				return "UNKNOWN";

			case TOKEN_WORD_DEFAULT:
				return "WORD_DEFAULT";
			case TOKEN_WORD_KEYWORD:
				return "WORD_KEYWORD";
			case TOKEN_WORD_IDENTIFIER:
				return "WORD_IDENTIFIER";

			case TOKEN_LITERAL_DEFAULT:
				return "LITERAL_DEFAULT";
			case TOKEN_LITERAL_DEFAULT_NUMBER:
				return "LITERAL_DEFAULT_NUMBER";
			case TOKEN_LITERAL_STRING:
				return "LITERAL_STRING";
			case TOKEN_LITERAL_INTEGER:
				return "LITERAL_INTEGER";
			case TOKEN_LITERAL_FLOAT:
				return "LITERAL_FLOAT";
			case TOKEN_LITERAL_CHAR:
				return "LITERAL_CHAR";
			case TOKEN_LITERAL_BOOLEAN:
				return "LITERAL_BOOLEAN";
			case TOKEN_LITERAL_NONE:
				return "LITERAL_NONE";

			case TOKEN_OPERATOR:
				return "OPERATOR";
			}
			return "DEFAULT";
		}

		void Token::setValue(std::string val)
		{
			value = val;
		}

		const std::string &Token::getValue() const
		{
			return value;
		}

		void Token::setFile(std::string val)
		{
			file = val;
		}

		const std::string &Token::getFile() const
		{
			return file;
		}

		void Token::setLine(unsigned int val)
		{
			line = val;
		}

		const unsigned int Token::getLine()
		{
			return line;
		}

		void Token::reset()
		{
			setType(TOKEN_DEFAULT);
			setCategory(TOKEN_CAT_DEFAULT);
			setValue("");
		}
	}
}
