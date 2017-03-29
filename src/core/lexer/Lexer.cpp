// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#include "core/lexer/Lexer.h"

namespace core
{
namespace lexer
{
    char Lexer::peekUpcoming(std::ptrdiff_t dist) const
    {
        return *(it + dist);
    }
    char Lexer::peekNext() const
    {
        return *(it + 1);
    }

    void Lexer::_next()
    {
        it++;
    }
    char Lexer::_getNext()
    {
        _next();
        assert(it.hasMore());
        return *it;
    }
    Lexer::ContentIterator& Lexer::advance()
    {
        _next();
        return it;
    }
    int Lexer::_advance()
    {
        _next(); // Next character

        if(!it.hasMore())
        {
            return 2;
        }
        if(*(it - 1) == '\n')
        {
            return 1;
        }
        return 0;
    }

    char Lexer::peekPrevious() const
    {
        return *(it - 1);
    }
    char Lexer::peekPassed(std::ptrdiff_t dist) const
    {
        return *(it - dist);
    }

    TokenVector Lexer::run()
    {
        TokenVector tokens;

        while(true)
        {
            Token t = getNextToken();
            if(getError())
            {
                break;
            }
            tokens.push_back(t);
            util::logger->trace("Pushed new token: ({}): '{}'",
                                t.typeToString(), t.value);
            if(t.type == TOKEN_EOF)
            {
                util::logger->trace("Token is EOF, stop");
                break;
            }
        }

        if(tokens.empty())
        {
            lexerError("Empty token list");
        }
        /*if(tokens.size() == 1 && tokens[0].type == TOKEN_EOF)
        {
            lexerWarning("Empty translation unit");
        }*/
        if(tokens.back().type != TOKEN_EOF && !getError())
        {
            lexerError("No EOF token found");
        }
        return tokens;
    }

    Token Lexer::createToken(TokenType type, const std::string& val) const
    {
        auto loc = it;
        const auto len = type == TOKEN_EOF ? 0 : val.length();
        loc.len = len;
        loc -= static_cast<std::ptrdiff_t>(len);

        assert(loc.it < loc.file->getContent().end() || *loc == '\0');
        assert(loc.it >= loc.file->getContent().begin());

        return Token(loc, type, val);
    }
} // namespace lexer
} // namespace core
