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
#include <locale>
#include <iostream>

#include "core/lexer/Token.h"
#include "util/StringUtils.h"
#include "util/Logger.h"

namespace core
{
	namespace lexer
	{
		bool Lexer::isKeyword(const std::string &buf)
		{
			if(
				// Control words, commands
				buf == "import" || buf == "function" || buf == "declare" ||
				buf == "export" || buf == "class" || buf == "override" ||
				buf == "final" || buf == "extend" || buf == "abstract" ||
				buf == "public" || buf == "protected" || buf == "private" ||
				buf == "if" || buf == "while" || buf == "for" || buf == "foreach" ||
				buf == "switch" || buf == "case" || buf == "break" ||
				buf == "return" || buf == "continue" ||
				buf == "true" || buf == "false" || buf == "none" ||

				// Datatypes
				buf == "None" || buf == "Void" || buf == "Integer" || buf == "Float" ||
				buf == "Double" || buf == "Decimal" || buf == "String" || buf == "Char" ||
				buf == "UInteger" || buf == "BigInteger" ||
				buf == "Int8" || buf == "Int16" || buf == "Int32" || buf == "Int64" ||
				buf == "UInt8" || buf == "UInt16" || buf == "UInt32" || buf == "UInt64" ||
				buf == "List" || buf == "Dict" || buf == "Array"
			)
			{
				return true;
			}
			return false;
		}

		bool Lexer::isWholeOperator(const std::string &buf)
		{
			if(
				buf == "+" || buf == "-" || buf == "*" || buf == "/" ||
				buf == "%" || buf == "^" || buf == "=" ||
				buf == "+=" || buf == "-=" || buf == "*=" || buf == "/=" ||
				buf == "%=" || buf == "^=" ||
				buf == "==" || buf == "<=" || buf == ">=" || buf == "!=" ||
				buf == "<" || buf == ">" ||
				buf == "!" || buf == "&&" || buf == "||" ||
				buf == "{" || buf == "}" || buf == "(" || buf == ")" ||
				buf == "[" || buf == "]" ||
				buf == ":" || buf == ";" || buf == "."
			)
			{
				return true;
			}
			return false;
		}

		bool Lexer::isBeginningOfOperator(const char &curr)
		{
			if(
				curr == '+' || curr == '-' || curr == '*' || curr == '/' ||
				curr == '%' || curr == '^' || curr == '=' ||
				curr == '<' || curr == '>' ||
				curr == '!' || curr == '&' || curr == '|' ||
				curr == '{' || curr == '}' || curr == '(' || curr == ')' ||
				curr == '[' || curr == ']' ||
				curr == ':' || curr == ';' || curr == '.'
			)
			{
				return true;
			}
			return false;
		}

		bool Lexer::isTerminatingOperator(const char &curr)
		{
			if(
				curr == '{' || curr == '}' || curr == '(' || curr == ')' ||
				curr == '[' || curr == ']' || curr == '"' ||
				curr == ':' || curr == ';' || curr == '.'
			)
			{
				return true;
			}
			return false;
		}

		bool Lexer::isWordOperator(const std::string &buf)
		{
			if(
				buf == "and" || buf == "or" || buf == "not" ||
				buf == "as" || buf == "of"
			)
			{
				return true;
			}
			return false;
		}

		TokenVector Lexer::run(const std::string &str, bool &error, const std::string &filename)
		{
			TokenVector tokens;
			std::string buffer;
			buffer.reserve(8);
			std::string::const_iterator strpointer = str.begin();
			const std::string::const_iterator strendpointer = str.end();
			Token currentToken;
			unsigned int currentLine = 1;
			error = false;

			for(; strpointer != strendpointer; ++strpointer)
			{
				const char currentChar = *strpointer;

				util::loggerBasic->trace("");
				util::logger->trace("Current character: '{}'", currentChar);

				if(currentChar == '\n')
				{
					++currentLine;
				}

				// Empty buffer
				// Either start of file, or a new token
				if(buffer.empty())
				{
					util::logger->trace("Buffer is empty");
					// Whitespace, skip
					if(util::StringUtils::isCharWhitespace(currentChar))
					{
						util::logger->trace("Current character is whitespace");
						continue;
					}
					// Number literals
					// '-' to detect negative numbers
					if(std::isdigit(currentChar) || currentChar == '-')
					{
						util::logger->trace("Current character is a number literal");
						currentToken.setCategory(TOKEN_CAT_LITERAL);
						currentToken.setType(TOKEN_LITERAL_DEFAULT_NUMBER);
						buffer.push_back(currentChar);
						util::logger->trace("Pushed the current character into the buffer. Buffer now: '{}'", buffer);
						continue;
					}
					// String literal
					if(currentChar == '"')
					{
						util::logger->trace("Current character is a string literal");
						currentToken.setCategory(TOKEN_CAT_LITERAL);
						currentToken.setType(TOKEN_LITERAL_STRING);
						buffer.push_back(currentChar);
						continue;
					}
					// Operator
					if(isBeginningOfOperator(currentChar))
					{
						util::logger->trace("Current character is a beginning of an operator");
						currentToken.setCategory(TOKEN_CAT_OPERATOR);
						currentToken.setType(TOKEN_OPERATOR);
						buffer.push_back(currentChar);
						util::logger->trace("Pushed the current character into the buffer. Buffer now: '{}'", buffer);
						continue;
					}
					// Word
					if(isalpha(currentChar) || currentChar == '_')
					{
						util::logger->trace("Current character is either a word");
						currentToken.setCategory(TOKEN_CAT_WORD);
						buffer.push_back(currentChar);
						util::logger->trace("Pushed current character into the buffer. Buffer now: '{}'", buffer);
						continue;
					}
					util::logger->trace("Current character is an unknown type");
					currentToken.setCategory(TOKEN_CAT_UNKNOWN);
					currentToken.setType(TOKEN_UNKNOWN);
					buffer.push_back(currentChar);
					util::logger->trace("Pushed token into the token list");
					continue;
				}
				// Buffer not empty
				else
				{
					util::logger->trace("The buffer is not empty. Buffer: '{}'", buffer);
					// Whitespace or terminating operator
					// Token over
					if(currentToken.getType() == TOKEN_LITERAL_STRING)
					{
						util::logger->trace("Current token is a string literal");

						const bool currentCharIsUnescapedQuotes = (currentChar == '"' && *(strpointer - 1) != '\\');

						if(*(strpointer - 1) == '\\')
						{
							if(currentChar == '"' || currentChar == '\\')
							{
								buffer.pop_back();
							}
						}

						if(currentCharIsUnescapedQuotes)
						{
							util::logger->trace("Current character is a '\"'");

							currentToken.setValue(buffer);
							tokens.push_back(currentToken);
							util::logger->trace("Pushed token into the token list");

							currentToken.reset();
							buffer.clear();
							util::logger->trace("Cleared buffer");

							continue;
						}

						if(buffer == "\"")
						{
							buffer.clear();
						}

						buffer.push_back(currentChar);
						util::logger->trace("Pushed current character into the buffer. Buffer: '{}'", buffer);

						continue;
					}
					else if(util::StringUtils::isCharWhitespace(currentChar) || isTerminatingOperator(currentChar))
					{
						util::logger->trace("Current character is either whitespace or a terminating operator");
						if(currentToken.getCategory() == TOKEN_CAT_WORD)
						{
							if(isWordOperator(buffer))
							{
								currentToken.setType(TOKEN_OPERATOR);
								currentToken.setCategory(TOKEN_CAT_OPERATOR);
								util::logger->trace("Current token is actually an operator");
							}
							else if(isKeyword(buffer))
							{
								currentToken.setType(TOKEN_WORD_KEYWORD);
								util::logger->trace("Current token is a keyword");
							}
							else
							{
								currentToken.setType(TOKEN_WORD_IDENTIFIER);
								util::logger->trace("Current token is an identifier");
							}
						}

						currentToken.setValue(buffer);
						tokens.push_back(currentToken);
						util::logger->trace("Pushed token into the token list");

						currentToken.reset();
						buffer.clear();
						util::logger->trace("Cleared buffer");

						if(isTerminatingOperator(currentChar))
						{
							currentToken.setCategory(TOKEN_CAT_OPERATOR);
							currentToken.setType(TOKEN_OPERATOR);
							currentToken.setValue(util::StringUtils::charToString(currentChar));
							tokens.push_back(currentToken);
							util::logger->trace("Pushed token into the token list");

							currentToken.reset();
						}
						continue;
					}
					else
					{
						util::logger->trace("Current character is not whitespace or a terminating operator");
						if(currentToken.getCategory() == TOKEN_CAT_WORD)
						{
							util::logger->trace("Current token is a word");
							if(isalnum(currentChar) || currentChar == '_')
							{
								buffer.push_back(currentChar);
								util::logger->trace("Pushed current character into the buffer. Buffer: '{}'", buffer);
								continue;
							}
							else
							{
								util::logger->error("Lexer Error: Invalid keyword/identifier: '{}'. On {}:{}", currentChar, filename, currentLine);
								error = true;
								return tokens;
							}
						}

						if(currentToken.getType() == TOKEN_LITERAL_DEFAULT_NUMBER)
						{
							util::logger->trace("Current token is a number literal");
							if(isdigit(currentChar))
							{
								buffer.push_back(currentChar);
								util::logger->trace("Pushed current character into the buffer. Buffer: '{}'", buffer);
								continue;
							}
							else
							{
								util::logger->error("Lexer Error: Invalid number literal: '{}'. On {}:{}", currentChar, filename, currentLine);
								error = true;
								return tokens;
							}
						}

						if(currentToken.getType() == TOKEN_LITERAL_STRING)
						{
							if(buffer == "\"")
							{
								buffer.clear();
							}
							util::logger->trace("Current token is a string literal");
							buffer.push_back(currentChar);
							util::logger->trace("Pushed current character into the buffer. Buffer: '{}'", buffer);
							continue;
						}

						if(currentToken.getCategory() == TOKEN_CAT_OPERATOR)
						{
							util::logger->trace("Current token is an operator");
							if(util::StringUtils::isCharWhitespace(*(strpointer + 1)))
							{
								buffer.push_back(currentChar);
							}
							currentToken.setValue(buffer);
							tokens.push_back(currentToken);
							currentToken.reset();
							buffer.clear();
							util::logger->trace("Pushed token into the token list");
							continue;
						}
					}
				}

				currentToken.setType(TOKEN_DEFAULT);
				if(buffer.empty())
				{
					currentToken.setValue(util::StringUtils::charToString(*strpointer));
				}
				else
				{
					currentToken.setValue(buffer);
				}
				tokens.push_back(currentToken);

				currentToken.reset();
				buffer.clear();
				continue;
			}

			if(!buffer.empty())
			{
				util::loggerBasic->trace("");
				util::logger->trace("Character loop finished, buffer still not empty: '{}'", buffer);
				if(currentToken.getType() != TOKEN_DEFAULT && currentToken.getType() != TOKEN_UNKNOWN)
				{
					util::logger->trace("Pushing token into the token list. Category: {}, Type: {}", currentToken.categoryToString(), currentToken.typeToString());
					currentToken.setValue(buffer);
					tokens.push_back(currentToken);
				}
			}

			util::logger->trace("Every character has been gone through\n");

			return tokens;
		}
	}
}
