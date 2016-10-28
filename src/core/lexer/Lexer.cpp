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
				buf == "import" || buf == "function" || buf == "Void" ||
				buf == "Integer"
			)
			{
				return true;
			}
			return false;
		}

		bool Lexer::isArgOperator(const char &curr)
		{
			switch (curr) {
			case '+':
			case '-':
			case '*':
			case '/':
			case '%':
			case '=':
				return true;
			}
			return false;
		}

		bool Lexer::isControlOperator(const char &curr)
		{
			switch(curr)
			{
			case '{':
			case '}':
			case '(':
			case ')':
			case ':':
			case ';':
				return true;
			}
			return false;
		}

		bool Lexer::isTerminatingOperator(const char &curr)
		{
			switch(curr)
			{
			case '{':
			case '}':
			case '(':
			case ')':
			case ';':
			case ':':
			case '"':
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
				/*Token t;
				t.setType(identifyBuffer(buffer));
				t.setType(TOKEN_DEFAULT);
				t.setValue(util::StringUtils::charToString(*strpointer));
				tokens.push_back(t);*/

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
					if(std::isdigit(currentChar) || *strpointer == '-')
					{
						util::logger->trace("Current character is a number literal");
						currentToken.setType(TOKEN_LITERAL_NUMBER);
						buffer.push_back(currentChar);
						util::logger->trace("Pushed the current character into the buffer. Buffer now: '{}'", buffer);
						continue;
					}
					// Control operator
					if(isControlOperator(currentChar))
					{
						util::logger->trace("Current character is a control operator");
						currentToken.setType(TOKEN_CONTROL_OPERATOR);
						buffer.push_back(currentChar);
						currentToken.setValue(buffer);
						tokens.push_back(currentToken);
						util::logger->trace("Pushed token into the token list");

						currentToken.reset();
						buffer.clear();
						util::logger->trace("Cleared buffer");
						continue;
					}
					// Argument operator
					if(isArgOperator(currentChar))
					{
						util::logger->trace("Current character is an argument operator");
						currentToken.setType(TOKEN_ARG_OPERATOR);
						buffer.push_back(currentChar);
						currentToken.setValue(buffer);
						tokens.push_back(currentToken);
						util::logger->trace("Pushed token into the token list");

						currentToken.reset();
						buffer.clear();
						util::logger->trace("Cleared buffer");
						continue;
					}
					if(isalpha(currentChar) || currentChar == '_')
					{
						util::logger->trace("Current character is either a keyword or an identifier");
						currentToken.setType(TOKEN_KEYWORD_OR_IDENTIFIER);
						buffer.push_back(currentChar);
						util::logger->trace("Pushed current character into the buffer. Buffer now: '{}'", buffer);
						continue;
					}
					util::logger->trace("Current character is an unknown type");
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
					if(util::StringUtils::isCharWhitespace(currentChar) || isTerminatingOperator(currentChar))
					{
						util::logger->trace("Current character is either whitespace or a terminating operator");
						if(currentToken.getType() == TOKEN_KEYWORD_OR_IDENTIFIER)
						{
							if(isKeyword(buffer))
							{
								currentToken.setType(TOKEN_KEYWORD);
								util::logger->trace("Current token is a keyword");
							}
							else
							{
								currentToken.setType(TOKEN_IDENTIFIER);
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
							util::logger->trace("Current character is a terminating operator");
							if(isControlOperator(currentChar))
							{
								util::logger->trace("Current character is a control operator");
								currentToken.setType(TOKEN_CONTROL_OPERATOR);
							}
							else if(isArgOperator(currentChar))
							{
								util::logger->trace("Current character is an argument operator");
								currentToken.setType(TOKEN_ARG_OPERATOR);
							}
							else
							{
								util::logger->error("Lexer Error: Invalid operator: '{}'. On {}:{}", currentChar, filename, currentLine);
								error = true;
								return tokens;
							}

							currentToken.setValue(util::StringUtils::charToString(currentChar));
							tokens.push_back(currentToken);
							util::logger->trace("Pushed token into the token list");

							currentToken.reset();
						}
						continue;
					}
					else
					{
						util::logger->trace("Current character is NOT whitespace or a terminating operator");
						if(currentToken.getType() == TOKEN_KEYWORD_OR_IDENTIFIER)
						{
							util::logger->trace("Current token is a keyword or an identifier");
							if(isalnum(currentChar) || currentChar == '_' || currentChar == '.')
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

						if(currentToken.getType() == TOKEN_LITERAL_NUMBER)
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

			util::logger->trace("\n");

			return tokens;
		}
	}
}
