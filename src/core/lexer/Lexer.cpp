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
#include <map>

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

			if(it == end)
			{
				util::logger->trace("Current character is EOF");
				return createToken(TOKEN_EOF, "EOF");
			}

			util::logger->trace("Getting next token. Current character: '{}', on line {}", currentChar, currentLine);

			// Skip any whitespace
			while(std::isspace(currentChar))
			{
				// Increase currentLine if current character is a newline character
				if(currentChar == '\n')
				{
					++currentLine;
				}

				util::logger->trace("Skipping whitespace");
				currentChar = *(++it);
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
				util::logger->trace("Word: '{}'", buf);

				// Identify the meaning of the word
				return getTokenFromWord(buf);
			}

			// Number literal
			// (-)[0-9][0-9\.]*
			if(
				std::isdigit(currentChar) ||
				(currentChar == '-' && std::isdigit(*(it + 1))) ||
				(currentChar == '.' && std::isdigit(*(it + 1)))
			)
			{
				std::string buf;
				buf.reserve(8);
				bool isFloatingPoint = (currentChar == '.');
				do
				{
					buf.push_back(currentChar);
					currentChar = *(++it);
					if(currentChar == '.')
					{
						isFloatingPoint = true;
					}
				} while(std::isdigit(currentChar) || currentChar == '.');
				util::logger->trace("Number literal: '{}'", buf);

				if(isFloatingPoint)
				{
					TokenType type;
					switch (currentChar)
					{
					case 'd':
						type = TOKEN_LITERAL_DECIMAL;
						break;
					case 'f':
						type = TOKEN_LITERAL_FLOAT;
						break;
					default:
						type = TOKEN_LITERAL_DOUBLE;
					}
					if(type != TOKEN_LITERAL_DOUBLE)
					{
						++it;
					}
					return createToken(type, buf);
				}
				else
				{
					TokenType type;
					switch(currentChar)
					{
					case 'u':
						type = TOKEN_LITERAL_UNSIGNED;
						break;
					case 'l':
						type = TOKEN_LITERAL_LONG;
						break;
					case 's':
						type = TOKEN_LITERAL_SHORT;
						break;
					case 'b':
						type = TOKEN_LITERAL_INT_BIN;
						break;
					case 'o':
						type = TOKEN_LITERAL_INT_OCT;
						break;
					case 'h':
						type = TOKEN_LITERAL_INT_HEX;
						break;
					default:
						type = TOKEN_LITERAL_INTEGER;
					}
					if(type != TOKEN_LITERAL_INTEGER)
					{
						++it;
					}
					return createToken(type, buf);
				}
			}

			// String literal
			// "."
			if(currentChar == '"')
			{
				std::string buf;
				buf.reserve(8);
				// Skip the opening quotation mark
				util::logger->trace("String literal");
				while((++it) != end)
				{
					currentChar = *it;
					char prev = *(it - 1);
					char next = *(it + 1);
					util::logger->trace("Current character: '{}', prev: '{}', next: '{}'", currentChar, prev, next);

					// Current char is a newline
					// String doesn't end, terminate
					if(currentChar == '\n')
					{
						lexerError("Invalid string: Closing quote not found");
						break;
					}
					if(prev == '\\')
					{

					}
					// Current char is a quotation mark
					if(currentChar == '"')
					{
						if(prev == '\\' && *(it - 2) != '\\')
						{
							// Remove the backslash
							buf.pop_back();
						}
						// Not escaped, end string
						else
						{
							util::logger->trace("Current character is an unescaped quotation mark, end");
							break;
						}
					}

					buf.push_back(currentChar);
					util::logger->trace("Pushed buffer: '{}'", buf);
				}

				std::map<std::string, std::string> patterns =
				{
					{ "\\\\", "\\" },
					{ "\\n", "\n" },
					{ "\\r", "\r" },
					{ "\\t", "\t" },
					{ "\\f", "\f" },
					{ "\\v", "\v" },
					{ "\\\"", "\"" },
				};
				for(const auto &p: patterns)
				{
					util::StringUtils::replaceAll(buf, p.first, p.second);
				}

				util::logger->trace("String literal: '{}'", buf);

				++it;
				return createToken(TOKEN_LITERAL_STRING, buf);
			}

			++it;
			return createToken(TOKEN_DEFAULT, util::StringUtils::charToString(currentChar));
		}

		TokenVector Lexer::run()
		{
			// Initialize variables
			TokenVector tokens;

			std::string::const_iterator strpointer = content.begin();

			while(true)
			{
				Token t = getNextToken(strpointer);
				if(getError())
				{
					break;
				}
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
