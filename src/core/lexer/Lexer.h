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

#include "utf8.h"

#include "core/lexer/Token.h"
#include "util/Logger.h"

namespace core
{
	namespace lexer
	{
		using TokenVector = std::vector<core::lexer::Token>;
		using char_t = int64_t;

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
			char_t peekUpcoming(std::ptrdiff_t) const
			{
				throw std::logic_error("Lexer::peekUpcoming() is unimplemented");
			}
			char_t peekNext() const
			{
				//return peekUpcoming(1);
				return static_cast<char_t>(utf8::peek_next(it, end));
			}

			char_t _next()
			{
				return static_cast<char_t>(utf8::next(it, end));
			}
			ContentIterator &advance()
			{
				_next();
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
						if(peekNext() != '\n')
						{
							_next();
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
			char_t peekPrevious() const
			{
				auto copy = it;
				return utf8::prior(copy, content.begin());
			}
			char_t peekPassed(std::ptrdiff_t n) const
			{
				for(int i = 1; i < n; ++i)
				{
					peekPrevious();
				}
				return peekPrevious();
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
