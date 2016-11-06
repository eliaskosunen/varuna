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
#include <sstream>

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
			// -[0-9]([0-9\.]*)[dfulsboh]
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
			// ".+"
			if(currentChar == '"')
			{
				std::string buf = lexStringLiteral(it, end);
				if(!getError())
				{
					util::logger->trace("String literal: '{}'", buf);

					return createToken(TOKEN_LITERAL_STRING, buf);
				}

			}

			// Character literal
			// '.'
			if(currentChar == '\'')
			{
				std::string buf = lexStringLiteral(it, end, true);
				if(!getError())
				{
					util::logger->trace("Character literal: '{}'", buf);

					return createToken(TOKEN_LITERAL_CHAR, buf);
				}
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

		std::string Lexer::lexStringLiteral(std::string::const_iterator &it, const std::string::const_iterator &end, bool isChar)
		{
			std::string buf;
			buf.reserve(isChar ? 2 : 8);
			const char quote = (isChar ? '\'' : '"');
			while((++it) != end)
			{
				char currentChar = *it;
				char prev = *(it - 1);
				char next = *(it + 1);
				util::logger->trace("Current character: '{}', prev: '{}', next: '{}'", currentChar, prev, next);

				// Current char is a newline
				// String doesn't end, terminate
				if(currentChar == '\n')
				{
					lexerError("Invalid {} literal: Closing quote not found", (isChar ? "character" : "string"));
					break;
				}
				// Current char is a quotation mark
				if(currentChar == quote)
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

			util::logger->trace("Buffer before escaping: '{}'", buf);
			std::string escaped;
			std::string::const_iterator rit = buf.begin();
			while(rit != buf.end())
			{
				char c = *rit++;
				if(c == '\\' && rit != buf.end())
				{
					switch(*rit++)
					{
					case '\\':
						c = '\\';
						break;
					case 'n':
						c = '\n';
						break;
					case 't':
						c = '\t';
						break;
					case 'r':
						c = '\r';
						break;
					case 'f':
						c = '\f';
						break;
					case 'v':
						c = '\v';
						break;
					case 'b':
						c = '\b';
						break;
					case 'a':
						c = '\a';
						break;
					case 'x':
					{
						std::string hex;
						if(!std::isxdigit(*rit))
						{
							lexerWarning("Invalid hexadecimal escape sequence: The first character after an \\x should be a number in hexadecimal, got '{}' instead", *rit);
							continue;
						}
						hex.push_back(*rit);
						if(std::isxdigit(*(rit + 1)))
						{
							++rit;
							hex.push_back(*rit);
						}
						++rit;
						std::istringstream hexcharss(hex);
						unsigned int ch;
						hexcharss >> std::hex >> ch;
						c = ch;
						break;
					}
					case 'o':
					{
						std::string oct;
						if(!util::StringUtils::isodigit(*rit))
						{
							lexerWarning("Invalid octal escape sequence: The first character after an \\o should be a number in octal, got '{}' instead", *rit);
							continue;
						}
						oct.push_back(*rit);
						if(util::StringUtils::isodigit(*(rit + 1)))
						{
							++rit;
							oct.push_back(*rit);
							if(util::StringUtils::isodigit(*(rit + 1)))
							{
								++rit;
								oct.push_back(*rit);
							}
							++rit;
						}
						else
						{
							++rit;
						}
						std::istringstream octcharss(oct);
						unsigned int ch;
						octcharss >> std::oct >> ch;
						c = ch;
						break;
					}
					case '"':
						if(!isChar)
						{
							c = '"';
						}
						break;
					case '\'':
						if(isChar)
						{
							c = '\'';
						}
						break;
					default:
						lexerWarning("Invalid escape sequence");
						continue;
					}
				}
				escaped.push_back(c);
			}

			if(isChar && escaped.length() > 1)
			{
				lexerError("Invalid character literal: Length more than 1: '{}'", escaped);
			}
			++it;
			return escaped;
		}

		Token Lexer::createToken(TokenType type, const std::string &val) const
		{
			return Token::create(type, val, currentLine, filename);
		}

		TokenType Lexer::getTokenTypeFromWord(const std::string &buf) const
		{
			// Keywords
			if(buf == "import")		return TOKEN_KEYWORD_IMPORT;
			if(buf == "function")	return TOKEN_KEYWORD_FUNCTION;
			if(buf == "declare")	return TOKEN_KEYWORD_DECLARE;
			if(buf == "class")		return TOKEN_KEYWORD_CLASS;
			if(buf == "override")	return TOKEN_KEYWORD_OVERRIDE;
			if(buf == "final")		return TOKEN_KEYWORD_FINAL;
			if(buf == "extend")		return TOKEN_KEYWORD_EXTEND;
			if(buf == "abstract")	return TOKEN_KEYWORD_ABSTRACT;
			if(buf == "implement")	return TOKEN_KEYWORD_IMPLEMENT;
			if(buf == "interface")	return TOKEN_KEYWORD_INTERFACE;
			if(buf == "public")		return TOKEN_KEYWORD_PUBLIC;
			if(buf == "protected")	return TOKEN_KEYWORD_PROTECTED;
			if(buf == "private")	return TOKEN_KEYWORD_PRIVATE;
			if(buf == "if")			return TOKEN_KEYWORD_IF;
			if(buf == "while")		return TOKEN_KEYWORD_WHILE;
			if(buf == "for")		return TOKEN_KEYWORD_FOR;
			if(buf == "foreach")	return TOKEN_KEYWORD_FOREACH;
			if(buf == "switch")		return TOKEN_KEYWORD_SWITCH;
			if(buf == "case")		return TOKEN_KEYWORD_CASE;
			if(buf == "break")		return TOKEN_KEYWORD_BREAK;
			if(buf == "return")		return TOKEN_KEYWORD_RETURN;
			if(buf == "continue")	return TOKEN_KEYWORD_CONTINUE;

			// Datatypes
			if(buf == "None")		return TOKEN_DATATYPE_NONE;
			if(buf == "Void")		return TOKEN_DATATYPE_VOID;
			if(buf == "Integer")	return TOKEN_DATATYPE_INTEGER;
			if(buf == "Float")		return TOKEN_DATATYPE_FLOAT;
			if(buf == "Double")		return TOKEN_DATATYPE_DOUBLE;
			if(buf == "Decimal")	return TOKEN_DATATYPE_DECIMAL;
			if(buf == "String")		return TOKEN_DATATYPE_STRING;
			if(buf == "Char")		return TOKEN_DATATYPE_CHAR;
			if(buf == "UInteger")	return TOKEN_DATATYPE_UINTEGER;
			if(buf == "BigInteger")	return TOKEN_DATATYPE_BIGINTEGER;
			if(buf == "Bool")		return TOKEN_DATATYPE_BOOL;
			if(buf == "Int8")		return TOKEN_DATATYPE_INT8;
			if(buf == "Int16")		return TOKEN_DATATYPE_INT16;
			if(buf == "Int32")		return TOKEN_DATATYPE_INT32;
			if(buf == "Int64")		return TOKEN_DATATYPE_INT64;
			if(buf == "UInt8")		return TOKEN_DATATYPE_UINT8;
			if(buf == "UInt16")		return TOKEN_DATATYPE_UINT16;
			if(buf == "UInt32")		return TOKEN_DATATYPE_UINT32;
			if(buf == "UInt64")		return TOKEN_DATATYPE_UINT64;
			if(buf == "List")		return TOKEN_DATATYPE_LIST;
			if(buf == "Dict")		return TOKEN_DATATYPE_DICT;
			if(buf == "Array")		return TOKEN_DATATYPE_ARRAY;

			// Literals
			if(buf == "true")		return TOKEN_LITERAL_TRUE;
			if(buf == "false")		return TOKEN_LITERAL_FALSE;
			if(buf == "none")		return TOKEN_LITERAL_NONE;

			// Textual operators
			if(buf == "and")		return TOKEN_OPERATORB_AND;
			if(buf == "or")			return TOKEN_OPERATORB_OR;
			if(buf == "not")		return TOKEN_OPERATORU_NOT;
			if(buf == "of")			return TOKEN_OPERATORB_OF;
			if(buf == "as")			return TOKEN_OPERATORB_AS;

			return TOKEN_IDENTIFIER;
		}

		Token Lexer::getTokenFromWord(const std::string &buf) const
		{
			return createToken(getTokenTypeFromWord(buf), buf);
		}
	}
}
