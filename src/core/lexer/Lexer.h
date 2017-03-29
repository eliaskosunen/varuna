// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#pragma once

#include "core/lexer/Token.h"
#include "util/File.h"
#include "util/IteratorUtils.h"
#include "util/Logger.h"
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
        using ContentIterator = util::SourceLocation;

        explicit Lexer(std::shared_ptr<util::File> f)
            : warningsAsErrors(false), content(f->getContent()),
              it(f, 1, 1, content.begin()), error(ERROR_NONE), file(f)
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
        enum LexCommentReturn
        {
            COMMENT_EOF,
            COMMENT_FOUND,
            COMMENT_NONE
        };

        TokenType getTokenTypeFromWord(const std::string& buf) const;
        Token getTokenFromWord(const std::string& buf) const;
        Token createToken(TokenType type, const std::string& val) const;

        Token getNextToken();

        std::string lexStringLiteral(bool isChar = false);

        TokenType getTokenTypeFromOperator(const std::string& buf) const;
        Token getTokenFromOperator(const std::string& buf) const;

        template <typename... Args>
        void lexerError(ContentIterator iter, const std::string& format,
                        Args&&... args);
        template <typename... Args>
        void lexerWarning(ContentIterator iter, const std::string& format,
                          Args&&... args);
        template <typename... Args>
        void lexerInfo(ContentIterator iter, const std::string& format,
                       Args&&... args);

        template <typename... Args>
        void lexerError(const std::string& format, Args&&... args);
        template <typename... Args>
        void lexerWarning(const std::string& format, Args&&... args);
        template <typename... Args>
        void lexerInfo(const std::string& format, Args&&... args);

        char peekUpcoming(std::ptrdiff_t dist) const;
        char peekNext() const;

        void _next();
        char _getNext();
        ContentIterator& advance();
        int _advance();

        char peekPrevious() const;
        char peekPassed(std::ptrdiff_t dist) const;

        LexCommentReturn lexComment();

        const std::string& content;
        ContentIterator it;

        ErrorLevel error;

        std::shared_ptr<util::File> file;
    };

    template <typename... Args>
    inline void Lexer::lexerError(ContentIterator iter,
                                  const std::string& format, Args&&... args)
    {
        error = ERROR_ERROR;
        util::logCompilerError(iter, format, std::forward<Args>(args)...);
    }
    template <typename... Args>
    inline void Lexer::lexerWarning(ContentIterator iter,
                                    const std::string& format, Args&&... args)
    {
        if(error != ERROR_ERROR)
        {
            error = ERROR_WARNING;
        }
        util::logCompilerWarning(iter, format, std::forward<Args>(args)...);
    }
    template <typename... Args>
    inline void Lexer::lexerInfo(ContentIterator iter,
                                 const std::string& format, Args&&... args)
    {
        util::logCompilerInfo(iter, format, std::forward<Args>(args)...);
    }

    template <typename... Args>
    inline void Lexer::lexerError(const std::string& format, Args&&... args)
    {
        lexerError(it, format, std::forward<Args>(args)...);
    }
    template <typename... Args>
    inline void Lexer::lexerWarning(const std::string& format, Args&&... args)
    {
        lexerWarning(it, format, std::forward<Args>(args)...);
    }
    template <typename... Args>
    inline void Lexer::lexerInfo(const std::string& format, Args&&... args)
    {
        lexerInfo(it, format, std::forward<Args>(args)...);
    }
} // namespace lexer
} // namespace core
