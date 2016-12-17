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
		using TokenVector = std::vector<core::lexer::Token>;
		using char_t = char;

		enum ErrorLevel
		{
			ERROR_NONE = 0,
			ERROR_WARNING,
			ERROR_ERROR
		};

		class Lexer final
		{
			const std::string &content;
			using ContentIterator = std::string::const_iterator;
			ContentIterator it;
			const ContentIterator end;

			SourceLocation currentLocation;

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
				if(error != ERROR_ERROR)
				{
					error = ERROR_WARNING;
				}
				util::logger->warn("{}: Lexer warning: {}", currentLocation.toString(), fmt::format(format, args...));
			}

			void newline()
			{
				currentLocation.line++;
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
				if(!lexComment())
				{
					return it;
				}

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

				return it;
			}

			void prevline()
			{
				currentLocation.line--;
			}
			ContentIterator peekPassed(std::ptrdiff_t i) const
			{
				return (it - i);
			}
			ContentIterator peekPrevious() const
			{
				return peekPassed(1);
			}

			bool lexComment();
		public:
			bool warningsAsErrors;

			Lexer(const std::string &cont, const std::string &file = "(undefined)")
				: content(cont), it(content.begin()), end(content.end()),
				currentLocation(file, 1), error(ERROR_NONE), warningsAsErrors(false) {}

			Lexer(const Lexer&) = delete;
			Lexer(Lexer&&) = default;
			Lexer &operator =(const Lexer&) = delete;
			Lexer &operator =(Lexer&&) = default;
			~Lexer() = default;

			TokenVector run();

			bool getError() const
			{
				if(error == ERROR_NONE)
				{
					return false;
				}
				if(warningsAsErrors)
				{
					return true;
				}
				if(error == ERROR_ERROR)
				{
					return true;
				}
				return false;
			}

			ErrorLevel getErrorLevel() const
			{
				return error;
			}
		};
	} // namespace lexer
} // namespace core
