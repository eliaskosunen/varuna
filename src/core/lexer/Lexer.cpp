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

#include "core/lexer/Token.h"
#include "util/StringUtils.h"

namespace core
{
	namespace lexer
	{
		bool Lexer::isKeyword(const std::string &buf)
		{
			if(
				buf == "import" || buf == "function" || buf == "void"
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

				// Empty buffer
				// Either start of file, or a new token
				if(buffer.empty())
				{
					// Whitespace, skip
					if(util::StringUtils::isCharWhitespace(currentChar))
					{
						continue;
					}
					// Number literals
					// '-' to detect negative numbers
					if(std::isdigit(currentChar) || *strpointer == '-')
					{
						currentToken.setType(TOKEN_LITERAL_NUMBER);
						buffer.push_back(currentChar);
						continue;
					}
					// Control operator
					if(isControlOperator(currentChar))
					{
						currentToken.setType(TOKEN_CONTROL_OPERATOR);
						buffer.push_back(currentChar);

						currentToken.reset();
						buffer.clear();
						continue;
					}
					// Argument operator
					if(isArgOperator(currentChar))
					{
						currentToken.setType(TOKEN_CONTROL_OPERATOR);
						buffer.push_back(currentChar);

						currentToken.reset();
						buffer.clear();
						continue;
					}
					if(isalnum(currentChar) || currentChar == '_')
					{
						currentToken.setType(TOKEN_KEYWORD_OR_IDENTIFIER);
						buffer.push_back(currentChar);
						continue;
					}
					currentToken.setType(TOKEN_UNKNOWN);
					buffer.push_back(currentChar);
					continue;
				}
				// Buffer not empty
				else
				{
					// Whitespace or terminating operator
					// Token over
					if(util::StringUtils::isCharWhitespace(currentChar) || isTerminatingOperator(currentChar))
					{
						if(currentToken.getType() == TOKEN_KEYWORD_OR_IDENTIFIER)
						{
							if(isKeyword(buffer))
							{
								currentToken.setType(TOKEN_KEYWORD);
							}
							else
							{
								currentToken.setType(TOKEN_IDENTIFIER);
							}
						}

						currentToken.setValue(buffer);
						tokens.push_back(currentToken);

						currentToken.reset();
						buffer.clear();

						if(isTerminatingOperator(currentChar))
						{
							if(isControlOperator(currentChar))
							{
								currentToken.setType(TOKEN_CONTROL_OPERATOR);
							}
							else if(isArgOperator(currentChar))
							{
								currentToken.setType(TOKEN_ARG_OPERATOR);
							}
							else
							{
								// TODO: Raise error for invalid operator
							}

							currentToken.setValue(util::StringUtils::charToString(currentChar));
							tokens.push_back(currentToken);

							currentToken.reset();
						}
						continue;
					}
					else
					{
						if(currentToken.getType() == TOKEN_KEYWORD_OR_IDENTIFIER)
						{
							if(isalnum(currentChar) || currentChar == '_')
							{
								buffer.push_back(currentChar);
								continue;
							}
							else
							{
								// TODO: Raise error for invalid keyword/identifier
							}
						}
					}
				}

				currentToken.setType(TOKEN_DEFAULT);
				currentToken.setValue(util::StringUtils::charToString(*strpointer));
				tokens.push_back(currentToken);

				currentToken.reset();
				buffer.clear();
				continue;
			}

			return tokens;
		}
	}
}
