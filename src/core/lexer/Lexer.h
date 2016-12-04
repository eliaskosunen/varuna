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
#include <vector>

#include "core/lexer/Token.h"
#include "util/Logger.h"

namespace core
{
	namespace lexer
	{
		typedef std::vector<core::lexer::Token> TokenVector;
		typedef char char_t;

		enum ErrorLevel
		{
			ERROR_NONE = 0,
			ERROR_WARNING,
			ERROR_ERROR
		};

		class Lexer
		{
			const std::string &content;
			typedef std::string::const_iterator ContentIterator;
			ContentIterator it;
			const ContentIterator end;

			SourceLocation currentLocation;
			uint64_t lastLineLen;

			ErrorLevel error;

			TokenType getTokenTypeFromWord(const std::string &buf) const;
			Token getTokenFromWord(const std::string &buf) const;
			Token createToken(TokenType type, const std::string &val) const;

			Token getNextToken();

			std::string lexStringLiteral(bool isChar = false);

			TokenType getTokenTypeFromOperator(const std::string &buf) const;
			Token getTokenFromOperator(const std::string &buf) const;

			template <typename... Args>
			void lexerError(const std::string &format, const Args& ... args)
			{
				error = ERROR_ERROR;
				util::logger->error("{}: Lexer error: {}", currentLocation.toString(), fmt::format(format, args...));
			}

			template <typename... Args>
			void lexerWarning(const std::string &format, const Args& ... args)
			{
				if(error != ERROR_ERROR) error = ERROR_WARNING;
				util::logger->warn("{}: Lexer warning: {}", currentLocation.toString(), fmt::format(format, args...));
			}

			void syntaxCheck(const TokenVector &tokens);

			void newline()
			{
				lastLineLen = currentLocation.column;
				currentLocation.line++;
				currentLocation.column = 1;
			}
			ContentIterator peekUpcoming(std::ptrdiff_t i) const
			{
				return (it + i);
			}
			ContentIterator peekNext() const
			{
				return peekUpcoming(1);
			}
			ContentIterator &advance()
			{
				++it;

				bool newLine = false;
				if(it != end)
				{
					if(*it == '\r')
					{
						newLine = true;
						// Skip '\r' if next is '\n'
						if(*peekNext() != '\n')
						{
							++it;
						}
					}
					if(*it == '\n')
					{
						newLine = true;
					}
				}

				if(newLine)
				{
					newline();
				}
				else
				{
					currentLocation.column++;
				}

				return it;
			}

			void prevline()
			{
				currentLocation.line--;
				currentLocation.column = lastLineLen;
				lastLineLen = 0;
			}
			ContentIterator peekPassed(std::ptrdiff_t i) const
			{
				return (it - i);
			}
			ContentIterator peekPrevious() const
			{
				return peekPassed(1);
			}
		public:
			bool warningsAsErrors;

			Lexer(const std::string &cont, const std::string &file = "(undefined)") : content(cont), it(content.begin()), end(content.end()), currentLocation(file, 1, 1), lastLineLen(0), error(ERROR_NONE), warningsAsErrors(false) {}

			TokenVector run();

			bool getError() const
			{
				if(error == ERROR_NONE) return false;
				if(warningsAsErrors) return true;
				if(error == ERROR_ERROR) return true;
				return false;
			}

			ErrorLevel getErrorLevel() const
			{
				return error;
			}
		};
	}
}
