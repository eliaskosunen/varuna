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
#include "util/StringUtils.h"

namespace core
{
	namespace lexer
	{
		Token Lexer::getNextToken(std::string::const_iterator &it)
		{
			const std::string::const_iterator end = content.end();
			char currentChar = *it;

			util::loggerBasic->trace("");
			util::logger->trace("Getting next token. Current character: '{}', on line {}", currentChar, currentLine);

			if(it == end)
			{
				return createToken(TOKEN_EOF, "EOF");
			}

			// Skip any whitespace
			while(std::isspace(currentChar))
			{
				currentChar = *(++it);

				// Increase currentLine if current character is a newline character
				if(currentChar == '\n')
				{
					++currentLine;
				}
			}

			// Word: a keyword or an identifier
			// [a-zA-Z_][a-zA-Z_0-9]*
			if(std::isalpha(currentChar) || currentChar == '_')
			{
				std::string buf;
				buf.reserve(8);
				buf.push_back(currentChar);
				while(std::isalnum(*(++it)) || *it == '_')
				{
					buf.push_back(*it);
				}

				// Identify the meaning of the word
				return getTokenFromWord(buf);
			}

			// Number literal
			if(std::isdigit(currentChar))
			{
				std::string buf;
				buf.reserve(8);
				do
				{
					buf.push_back(currentChar);
					currentChar = *(++it);
				} while(std::isdigit(currentChar) || currentChar == '.');

				return createToken(TOKEN_LITERAL_NUMBER, buf);
			}

			++it;
			return createToken(TOKEN_DEFAULT, util::StringUtils::charToString(currentChar));
		}

		TokenVector Lexer::run(bool &error)
		{
			// Initialize variables
			TokenVector tokens;
			error = false;

			std::string::const_iterator strpointer = content.begin();

			while(true)
			{
				Token t = getNextToken(strpointer);
				tokens.push_back(t);
				util::logger->trace("Pushed new token: ({}): '{}'", t.typeToString(), t.value);
				if(t.type == TOKEN_EOF)
				{
					util::logger->trace("Token is EOF, stop");
					break;
				}
			}

			return tokens;
		}

		Token Lexer::createToken(TokenType type, const std::string &val) const
		{
			return Token::create(type, val, currentLine, filename);
		}

		TokenType Lexer::getTokenTypeFromWord(const std::string &buf) const
		{
			if(buf == "import") return TOKEN_KEYWORD_IMPORT;
			return TOKEN_IDENTIFIER;
		}

		Token Lexer::getTokenFromWord(const std::string &buf) const
		{
			return createToken(getTokenTypeFromWord(buf), buf);
		}
	}
}
