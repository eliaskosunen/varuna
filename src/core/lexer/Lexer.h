// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#pragma once

#include "core/lexer/Token.h"
#include "util/IteratorUtils.h"
#include "util/Logger.h"
#include "util/String.h"
#include <utf8.h>
#include <string>
#include <vector>

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
    public:
        using ContentIterator = std::string::const_iterator;

        Lexer(util::string_t cont, const util::string_t& file = "(undefined)")
            : warningsAsErrors(false), content(std::move(cont)),
              it(content.begin()), end(content.end()), currentLocation(file, 1),
              error(ERROR_NONE)
        {
        }

        Lexer(const Lexer&) = delete;
        Lexer(Lexer&&) = default;
        Lexer& operator=(const Lexer&) = delete;
        Lexer& operator=(Lexer&&) = default;
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

        bool warningsAsErrors;

    private:
        TokenType getTokenTypeFromWord(const util::string_t& buf) const;
        Token getTokenFromWord(const util::string_t& buf) const;
        Token createToken(TokenType type, const util::string_t& val) const;

        Token getNextToken();

        util::string_t lexStringLiteral(bool isChar = false);

        TokenType getTokenTypeFromOperator(const util::string_t& buf) const;
        Token getTokenFromOperator(const util::string_t& buf) const;

        template <typename... Args>
        void lexerError(const util::string_t& format, const Args&... args);
        template <typename... Args>
        void lexerWarning(const util::string_t& format, const Args&... args);

        void newline();
        util::char_t peekUpcoming(std::ptrdiff_t) const;
        util::char_t peekNext() const;

        util::char_t _next();
        ContentIterator& advance();

        void prevline();
        util::char_t peekPrevious() const;
        util::char_t peekPassed(std::ptrdiff_t n) const;

        bool lexComment();

        util::string_t content;
        ContentIterator it;
        ContentIterator end;

        SourceLocation currentLocation;

        ErrorLevel error;
    };

    template <typename... Args>
    inline void Lexer::lexerError(const util::string_t& format,
                                  const Args&... args)
    {
        error = ERROR_ERROR;
        util::logger->error("{}: Lexer error: {}", currentLocation.toString(),
                            fmt::format(format, args...));
    }

    template <typename... Args>
    inline void Lexer::lexerWarning(const util::string_t& format,
                                    const Args&... args)
    {
        if(error != ERROR_ERROR)
        {
            error = ERROR_WARNING;
        }
        util::logger->warn("{}: Lexer warning: {}", currentLocation.toString(),
                           fmt::format(format, args...));
    }

    inline void Lexer::newline()
    {
        currentLocation.line++;
    }
    inline util::char_t Lexer::peekUpcoming(std::ptrdiff_t) const
    {
        throw std::logic_error("Lexer::peekUpcoming() is unimplemented");
    }
    inline util::char_t Lexer::peekNext() const
    {
        // return peekUpcoming(1);
        // return static_cast<util::char_t>(utf8::peek_next(it, end));
        auto copy = it;
        return static_cast<util::char_t>(*(copy + 1));
    }

    inline util::char_t Lexer::_next()
    {
        // auto tmp = static_cast<util::char_t>(utf8::next(it, end));
        it += 1;
        auto tmp = static_cast<util::char_t>(*it);
        // util::string_t tmpstr("\0");
        // <3 STL iterators
        // plz ranges come fast
        // auto tmpit = std::find_end(content.rbegin(), content.rend(),
        // tmpstr.begin(), tmpstr.end());
        // end = util::ritToFwdIt(tmpit);
        return tmp;
    }
    inline Lexer::ContentIterator& Lexer::advance()
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

    inline void Lexer::prevline()
    {
        currentLocation.line--;
    }
    inline util::char_t Lexer::peekPrevious() const
    {
        auto copy = it;
        // return static_cast<util::char_t>(utf8::prior(copy,
        // content.begin()));
        return static_cast<util::char_t>(*(copy - 1));
    }
    inline util::char_t Lexer::peekPassed(std::ptrdiff_t n) const
    {
        for(int i = 1; i < n; ++i)
        {
            peekPrevious();
        }
        return peekPrevious();
    }
} // namespace lexer
} // namespace core
