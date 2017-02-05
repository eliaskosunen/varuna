// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#pragma once

#include "core/lexer/Token.h"
#include "util/File.h"
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

        explicit Lexer(std::shared_ptr<util::File> f)
            : warningsAsErrors(false), content(f->getContent()),
              it(content.begin()), end(content.end()),
              currentLocation(f->getFilename(), 1, 1), error(ERROR_NONE),
              file(f)
        {
        }

        Lexer(const Lexer&) = delete;
        Lexer(Lexer&&) = default;
        Lexer& operator=(const Lexer&) = delete;
        Lexer& operator=(Lexer&&) = default;
        ~Lexer() noexcept = default;

        /// Run lexer
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
        void lexerError(const util::string_t& format, Args&&... args);
        template <typename... Args>
        void lexerWarning(const util::string_t& format, Args&&... args);

        void newline();
        util::char_t peekUpcoming(std::ptrdiff_t) const;
        util::char_t peekNext() const;

        void _next();
        util::char_t _getNext();
        ContentIterator& advance();
        int _advance();

        [[noreturn]] void prevline();
        util::char_t peekPrevious() const;
        util::char_t peekPassed(std::ptrdiff_t n) const;

        bool lexComment();

        util::string_t content;
        ContentIterator it;
        ContentIterator end;

        util::SourceLocation currentLocation;

        ErrorLevel error;

        std::shared_ptr<util::File> file;
    };

    template <typename... Args>
    inline void Lexer::lexerError(const util::string_t& format, Args&&... args)
    {
        error = ERROR_ERROR;
        util::logCompilerError(file.get(), currentLocation, format,
                               std::forward<Args>(args)...);
    }

    template <typename... Args>
    inline void Lexer::lexerWarning(const util::string_t& format,
                                    Args&&... args)
    {
        if(error != ERROR_ERROR)
        {
            error = ERROR_WARNING;
        }
        util::logCompilerWarning(file.get(), currentLocation, format,
                                 std::forward<Args>(args)...);
    }

    inline void Lexer::newline()
    {
        currentLocation.line++;
        currentLocation.col = 1;
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

    inline void Lexer::_next()
    {
        it += 1;
        if(it != end)
        {
            if(*it == '\t')
            {
                currentLocation.col += 4;
            }
            else
            {
                currentLocation.col++;
            }
        }
    }
    inline util::char_t Lexer::_getNext()
    {
        // auto tmp = static_cast<util::char_t>(utf8::next(it, end));
        _next();
        assert(it != end);
        auto tmp = static_cast<util::char_t>(*it);
        return tmp;
    }
    inline Lexer::ContentIterator& Lexer::advance()
    {
        if(it + 1 == end)
        {
            _next();
            return end;
        }
        _next();
        if(!lexComment())
        {
            return it;
        }

        if(*it == '\r')
        {
            if((it + 1) != end && peekNext() != '\n')
            {
                // Error message formatting would go out of bounds
                // if we did it
                // Throwing instead
                // TODO: better solution
                throw std::runtime_error("Unexpected CR (carriage return) "
                                         "without a trailing LF (line feed)");
            }
            util::logger->trace("Found \\r, skipping.");
            _next();
        }

        if(*it == '\n')
        {
            util::logger->trace("Found \\n, skipping.");
            newline();
            _next(); // Skip '\n'
            return it;
        }

        return it;
    }
    inline int Lexer::_advance()
    {
        _next(); // Next character

        if(it == end)
        {
            return 2;
        }

        if(*it == '\r')
        {
            if(peekNext() != '\n')
            {
                lexerWarning("Unexpected CR (carriage return) "
                             "without a trailing LF (line feed)");
                newline();
                return 1;
            }
            _next(); // Skip '\r'
        }

        if(*it == '\n')
        {
            newline();
            return 1;
        }
        return 0;
    }

    inline void Lexer::prevline()
    {
        currentLocation.line--;
        throw std::logic_error(
            "Don't use Lexer::prevline(), it messes up the column counter");
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
