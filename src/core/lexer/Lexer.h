/*
Copyright (C) 2016-2017 Elias Kosunen

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
#include "util/IteratorUtils.h"
#include "util/String.h"

namespace core
{
	namespace lexer
	{
		using TokenVector = std::vector<core::lexer::Token>;

		enum ErrorLevel
		{
			ERROR_NONE = 0,
			ERROR_WARNING,
			ERROR_ERROR
		};

		class Lexer final
		{
			util::string_t content;
			using ContentIterator = std::string::const_iterator;
			ContentIterator it;
			ContentIterator end;

			SourceLocation currentLocation;

			ErrorLevel error;

			TokenType getTokenTypeFromWord(const util::string_t &buf) const;
			Token getTokenFromWord(const util::string_t &buf) const;
			Token createToken(TokenType type, const util::string_t &val) const;

			Token getNextToken();

			util::string_t lexStringLiteral(bool isChar = false);

			TokenType getTokenTypeFromOperator(const util::string_t &buf) const;
			Token getTokenFromOperator(const util::string_t &buf) const;

			template <typename... Args>
			void lexerError(const util::string_t &format, const Args& ... args)
			{
				error = ERROR_ERROR;
				util::logger->error("{}: Lexer error: {}", currentLocation.toString(), fmt::format(format, args...));
			}

			template <typename... Args>
			void lexerWarning(const util::string_t &format, const Args& ... args)
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
			util::char_t peekUpcoming(std::ptrdiff_t) const
			{
				throw std::logic_error("Lexer::peekUpcoming() is unimplemented");
			}
			util::char_t peekNext() const
			{
				//return peekUpcoming(1);
				//return static_cast<util::char_t>(utf8::peek_next(it, end));
				auto copy = it;
				return static_cast<util::char_t>(*(copy + 1));
			}

			util::char_t _next()
			{
				//auto tmp = static_cast<util::char_t>(utf8::next(it, end));
				it += 1;
				auto tmp = static_cast<util::char_t>(*it);
				//util::string_t tmpstr("\0");
				// <3 STL iterators
				// plz ranges come fast
				//auto tmpit = std::find_end(content.rbegin(), content.rend(), tmpstr.begin(), tmpstr.end());
				//end = util::ritToFwdIt(tmpit);
				return tmp;
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
			util::char_t peekPrevious() const
			{
				auto copy = it;
				//return static_cast<util::char_t>(utf8::prior(copy, content.begin()));
				return static_cast<util::char_t>(*(copy - 1));
			}
			util::char_t peekPassed(std::ptrdiff_t n) const
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

			static util::string_t addPadding(util::string_t str)
			{
				const size_t endPadding = 4;
				str.push_back('\n');
				for(size_t i = 1; i <= endPadding; ++i)
				{
					str.push_back(' ');
				}
				return str;
			}

			Lexer(util::string_t cont, const util::string_t &file = "(undefined)")
				: content(addPadding(std::move(cont))),
				it(content.begin()), end(content.end()),
				currentLocation(file, 1), error(ERROR_NONE), warningsAsErrors(false)
			{}

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
