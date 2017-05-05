// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#include "core/lexer/Lexer.h"
#include "util/StringUtils.h"

namespace core
{
namespace lexer
{
    Token Lexer::getNextToken()
    {
        char currentChar = *it;
        util::loggerBasic->trace("");

        while(true)
        {
            if(!it.hasMore())
            {
                return createToken(TOKEN_EOF, "EOF");
            }

            bool match = false;
            currentChar = *it;
            if(util::stringutils::isCharWhitespace(currentChar))
            {
                util::logger->trace("Skipping whitespace: '{}'", currentChar);

                advance();
                if(!it.hasMore())
                {
                    return createToken(TOKEN_EOF, "EOF");
                }

                currentChar = *it;
                util::logger->trace("currentChar after advancing: '{}'",
                                    currentChar);

                match = true;
            }

            auto comment = lexComment();
            if(comment == COMMENT_EOF)
            {
                return createToken(TOKEN_EOF, "EOF");
            }
            if(comment == COMMENT_FOUND)
            {
                match = true;
            }

            if(!match)
            {
                break;
            }
        }

        util::logger->trace(
            "Getting next token. Current character: '{}', on {}", currentChar,
            it.toString());

        // Invalid character
        if(util::stringutils::isCharControlCharacter(currentChar) &&
           !util::stringutils::isCharWhitespace(currentChar))
        {
            lexerWarning("Unrecognized character: '{}' ({}), skipped",
                         currentChar, static_cast<int32_t>(currentChar));
            advance();
            return createToken(TOKEN_DEFAULT,
                               util::stringutils::charToString(currentChar));
        }

        // Word: a keyword or an identifier
        // [a-zA-Z_][a-zA-Z_0-9]*
        if(util::stringutils::isValidIdentifierBeginningChar(currentChar))
        {
            // bchar
            if((currentChar == 'b' || currentChar == 'B') && peekNext() == '\'')
            {
                advance(); // Skip 'b'
                std::string buf = lexStringLiteral(true);
                if(!getError())
                {
                    Token t = createToken(TOKEN_LITERAL_CHAR, buf);
                    t.modifierChar = CHAR_BYTE;
                    return t;
                }
            }
            // cstring
            else if((currentChar == 'c' || currentChar == 'C') &&
                    peekNext() == '"')
            {
                advance(); // Skip 'c'
                std::string buf = lexStringLiteral(false);
                if(!getError())
                {
                    Token t = createToken(TOKEN_LITERAL_STRING, buf);
                    t.modifierString = STRING_C;
                    return t;
                }
            }
            else
            {
                std::string buf;
                buf.reserve(8);
                buf.push_back(currentChar);
                while(util::stringutils::isValidIdentifierChar(*advance()))
                {
                    buf.push_back(*it);
                }

                // Identify the meaning of the word
                return getTokenFromWord(buf);
            }
        }

        // Number literal
        // -?[0-9]([0-9\.]*)[dfulsboh]
        if(util::stringutils::isCharDigit(currentChar) ||
           (currentChar == '.' && util::stringutils::isCharDigit(*(it + 1))))
        {
            std::string buf;
            buf.reserve(8);
            bool isFloatingPoint = (currentChar == '.');
            enum _IntegerLiteralBase
            {
                BASE_DEC = 10,
                BASE_BIN = 2,
                BASE_OCT = 8,
                BASE_HEX = 16
            };
            using IntegerLiteralBase = util::SafeEnum<_IntegerLiteralBase>;
            IntegerLiteralBase base = BASE_DEC;
            if(currentChar == '0')
            {
                // 0x... = hexadecimal
                if(*(it + 1) == 'x' || *(it + 1) == 'X')
                {
                    _next();
                    currentChar = _getNext();
                    base = BASE_HEX;
                }
                // 0o... = octal
                else if(*(it + 1) == 'o' || *(it + 1) == 'O')
                {
                    _next();
                    currentChar = _getNext();
                    base = BASE_OCT;
                }
                // 0b... = binary
                else if(*(it + 1) == 'b' || *(it + 1) == 'B')
                {
                    _next();
                    currentChar = _getNext();
                    base = BASE_BIN;
                }
            }
            bool cont = true;
            do
            {
                buf.push_back(currentChar);
                currentChar = *advance();
                if(currentChar == '.')
                {
                    isFloatingPoint = true;
                }
                cont = [base, currentChar]() {
                    switch(base.get())
                    {
                    case BASE_DEC:
                        return util::stringutils::isCharDigit(currentChar);
                    case BASE_BIN:
                        return util::stringutils::isCharBinDigit(currentChar);
                    case BASE_OCT:
                        return util::stringutils::isCharOctDigit(currentChar);
                    case BASE_HEX:
                        return util::stringutils::isCharHexDigit(currentChar);
                    }
                    throw std::logic_error(
                        fmt::format("Unknown integer case: {}", base.get()));
                }();
            } while(cont || currentChar == '.');

            if(!isFloatingPoint)
            {
                Token t = createToken(TOKEN_LITERAL_INTEGER, buf);
                std::unordered_map<std::string, decltype(INTEGER_NONE)>
                    allowedModifiers = {{"i64", INTEGER_INT64},
                                        {"i32", INTEGER_INT32},
                                        {"i16", INTEGER_INT16},
                                        {"i8", INTEGER_INT8},
                                        {"o", INTEGER_BYTE}}; // o = octet
                auto mod = [&]() {
                    std::string modbuf;
                    while(util::stringutils::isCharAlnum(currentChar))
                    {
                        modbuf.push_back(currentChar);
                        auto modit = allowedModifiers.find(modbuf);
                        if(modit != allowedModifiers.end())
                        {
                            t.modifierInt |= modit->second;
                            allowedModifiers.erase(modit);
                            modbuf.clear();
                        }
                        advance();
                        if(!it.hasMore())
                        {
                            break;
                        }
                        currentChar = *it;
                    }
                    return modbuf;
                }();
                if(!mod.empty())
                {
                    lexerError("Invalid integer suffix: '{}'", mod);
                }
                if(t.modifierInt == INTEGER_NONE)
                {
                    t.modifierInt |= INTEGER_INT32;
                }

                if(base == BASE_BIN)
                {
                    t.modifierInt |= INTEGER_BIN;
                }
                else if(base == BASE_OCT)
                {
                    t.modifierInt |= INTEGER_OCT;
                }
                else if(base == BASE_HEX)
                {
                    t.modifierInt |= INTEGER_HEX;
                }
                return t;
            }

            Token t = createToken(TOKEN_LITERAL_FLOAT, buf);
            std::unordered_map<std::string, decltype(FLOAT_NONE)>
                allowedModifiers = {{"f32", FLOAT_F32},
                                    {"f64", FLOAT_F64},
                                    {"d", FLOAT_DECIMAL},
                                    {"l", FLOAT_LONG}};
            auto mod = [&]() {
                std::string modbuf;
                while(util::stringutils::isCharAlnum(currentChar))
                {
                    modbuf.push_back(currentChar);
                    auto modit = allowedModifiers.find(modbuf);
                    if(modit != allowedModifiers.end())
                    {
                        t.modifierFloat |= modit->second;
                        allowedModifiers.erase(modit);
                        modbuf.clear();
                    }
                    currentChar = *advance();
                }
                return modbuf;
            }();
            if(!mod.empty())
            {
                lexerError("Invalid float suffix: '{}'", mod);
            }
            if(t.modifierFloat == FLOAT_NONE)
            {
                t.modifierFloat |= FLOAT_F64;
            }
            if(base != BASE_DEC)
            {
                lexerError("Floating-point number can only be in decimal case");
            }
            return t;
        }

        // String literal
        // ".+"
        if(currentChar == '"')
        {
            std::string buf = lexStringLiteral(false);
            if(!getError())
            {
                Token t = createToken(TOKEN_LITERAL_STRING, buf);
                t.modifierString = STRING_STRING;
                return t;
            }
        }

        // Character literal
        // '.'
        if(currentChar == '\'')
        {
            std::string buf = lexStringLiteral(true);
            if(!getError())
            {
                Token t = createToken(TOKEN_LITERAL_CHAR, buf);
                t.modifierChar = CHAR_CHAR;
                return t;
            }
        }

        // Operator or punctuator
        if(util::stringutils::isCharPunctuation(currentChar))
        {
            std::string buf;
            while(util::stringutils::isCharPunctuation(currentChar))
            {
                if(getTokenTypeFromOperator(buf) != TOKEN_UNDEFINED)
                {
                    std::string tmp(buf);
                    tmp.push_back(*it);
                    if(getTokenTypeFromOperator(tmp) == TOKEN_UNDEFINED)
                    {
                        break;
                    }
                }
                buf.push_back(currentChar);
                if(_advance() != 0)
                {
                    break;
                }
                currentChar = *it;
            }
            Token t = getTokenFromOperator(buf);
            if(t.type == TOKEN_UNDEFINED)
            {
                lexerError(it - static_cast<ptrdiff_t>(buf.length()),
                           "Invalid operator or punctuator: '{}'", buf);
            }
            return t;
        }

        lexerWarning("Unrecognized token: '{}' ({})", currentChar,
                     static_cast<int32_t>(currentChar));
        advance();
        return createToken(TOKEN_DEFAULT,
                           util::stringutils::charToString(currentChar));
    }
} // namespace lexer
} // namespace core
