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

		TokenVector Lexer::run(const std::string &str)
		{
			TokenVector tokens;
			std::string buffer;
			buffer.reserve(8);
			std::string::const_iterator strpointer = str.begin();
			const std::string::const_iterator strendpointer = str.end();
			Token currentToken;

			for(; strpointer != strendpointer; ++strpointer)
			{
				/*Token t;
				t.setType(identifyBuffer(buffer));
				t.setType(TOKEN_DEFAULT);
				t.setValue(util::StringUtils::charToString(*strpointer));
				tokens.push_back(t);*/

				const char currentChar = *strpointer;

				
				std::cout << "\ncurrentChar: '" << currentChar << "'\n";

				// Empty buffer
				// Either start of file, or a new token
				if(buffer.empty())
				{
					std::cout << "buffer empty: '" << buffer << "'\n";
					// Whitespace, skip
					if(util::StringUtils::isCharWhitespace(currentChar))
					{
						std::cout << "currentChar is whitespace: '" << currentChar << "'\n";
						continue;
					}
					// Number literals
					// '-' to detect negative numbers
					if(std::isdigit(currentChar) || *strpointer == '-')
					{
						std::cout << "currentChar is number literal: '" << currentChar << "'\n";
						currentToken.setType(TOKEN_LITERAL_NUMBER);
						buffer.push_back(currentChar);
						std::cout << "pushed buffer: '" << buffer << "'\n";
						continue;
					}
					// Control operator
					if(isControlOperator(currentChar))
					{
						std::cout << "currentChar is control operator: '" << currentChar << "'\n";
						currentToken.setType(TOKEN_CONTROL_OPERATOR);
						buffer.push_back(currentChar);
						currentToken.setValue(buffer);
						tokens.push_back(currentToken);
						std::cout << "pushed buffer and token: '" << buffer << "'\n";

						currentToken.reset();
						buffer.clear();
						std::cout << "cleared buffer: '" << buffer << "'\n";
						continue;
					}
					// Argument operator
					if(isArgOperator(currentChar))
					{
						std::cout << "currentChar is arg operator: '" << currentChar << "'\n";
						currentToken.setType(TOKEN_ARG_OPERATOR);
						buffer.push_back(currentChar);
						currentToken.setValue(buffer);
						tokens.push_back(currentToken);
						std::cout << "pushed buffer and token: '" << buffer << "'\n";

						currentToken.reset();
						buffer.clear();
						std::cout << "cleared buffer: '" << buffer << "'\n";
						continue;
					}
					if(isalpha(currentChar) || currentChar == '_')
					{
						std::cout << "currentChar is keyword or identifier: '" << currentChar << "'\n";
						currentToken.setType(TOKEN_KEYWORD_OR_IDENTIFIER);
						buffer.push_back(currentChar);
						std::cout << "pushed buffer: '" << buffer << "'\n";
						continue;
					}
					std::cout << "currentChar is unknown: '" << currentChar << "'\n";
					currentToken.setType(TOKEN_UNKNOWN);
					buffer.push_back(currentChar);
					std::cout << "pushed buffer: '" << buffer << "'\n";
					continue;
				}
				// Buffer not empty
				else
				{
					std::cout << "buffer not empty: '" << buffer << "'\n";
					// Whitespace or terminating operator
					// Token over
					if(util::StringUtils::isCharWhitespace(currentChar) || isTerminatingOperator(currentChar))
					{
						std::cout << "currentChar is whitespace or terminating operator: '" << currentChar << "'\n";
						if(currentToken.getType() == TOKEN_KEYWORD_OR_IDENTIFIER)
						{
							if(isKeyword(buffer))
							{
								currentToken.setType(TOKEN_KEYWORD);
								std::cout << "currentToken is keyword: '" << buffer << "'\n";
							}
							else
							{
								currentToken.setType(TOKEN_IDENTIFIER);
								std::cout << "currentToken is identifier: '" << buffer << "'\n";
							}
						}

						currentToken.setValue(buffer);
						tokens.push_back(currentToken);
						std::cout << "pushed token: '" << buffer << "'\n";

						currentToken.reset();
						buffer.clear();
						std::cout << "cleared buffer: '" << buffer << "'\n";

						if(isTerminatingOperator(currentChar))
						{
							std::cout << "currentChar is terminating operator: '" << currentChar << "'\n";
							if(isControlOperator(currentChar))
							{
								std::cout << "currentChar is control operator: '" << currentChar << "'\n";
								currentToken.setType(TOKEN_CONTROL_OPERATOR);
							}
							else if(isArgOperator(currentChar))
							{
								std::cout << "currentChar is arg operator: '" << currentChar << "'\n";
								currentToken.setType(TOKEN_ARG_OPERATOR);
							}
							else
							{
								std::cout << "invalid operator: '" << currentChar << "'\n";
								// TODO: Raise error for invalid operator
							}

							currentToken.setValue(util::StringUtils::charToString(currentChar));
							tokens.push_back(currentToken);
							std::cout << "pushed token: '" << currentChar << "'\n";

							currentToken.reset();
						}
						continue;
					}
					else
					{
						std::cout << "currentChar is not whitespace or terminating operator: '" << currentChar << "'\n";
						if(currentToken.getType() == TOKEN_KEYWORD_OR_IDENTIFIER)
						{
							std::cout << "currentToken is keyword/identifier: '" << buffer << "'\n";
							if(isalnum(currentChar) || currentChar == '_' || currentChar == '.')
							{
								buffer.push_back(currentChar);
								std::cout << "pushed buffer: '" << buffer << "'\n";
								continue;
							}
							else
							{
								std::cout << "invalid keyword/identifier: '" << currentChar << "'\n";
								// TODO: Raise error for invalid keyword/identifier
								continue;
							}
						}

						if(currentToken.getType() == TOKEN_LITERAL_NUMBER)
						{
							std::cout << "currentToken is number literal: '" << buffer << "'\n";
							if(isdigit(currentChar))
							{
								buffer.push_back(currentChar);
								std::cout << "pushed buffer: '" << buffer << "'\n";
								continue;
							}
							else
							{
								std::cout << "invalid number: '" << currentChar << "'\n";
								// TODO: Raise error for number
								continue;
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

			std::cout << "\n";

			return tokens;
		}
	}
}
