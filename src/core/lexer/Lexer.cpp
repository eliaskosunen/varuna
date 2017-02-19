// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#include "core/lexer/Lexer.h"
#include "util/Logger.h"
#include "util/StringUtils.h"
#include <algorithm>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

namespace core
{
namespace lexer
{
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

    Token Lexer::getNextToken()
    {
        util::char_t currentChar = *it;
        util::loggerBasic->trace("");

        while(true)
        {
            util::logger->trace("In loop");
            if(it == end)
            {
                return createToken(TOKEN_EOF, "EOF");
            }

            bool match = false;
            currentChar = *it;
            if(util::stringutils::isCharWhitespace(currentChar))
            {
                util::logger->trace("Skipping whitespace: '{}'", currentChar);

                if(advance() == end)
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
            currentLocation.toString());

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
                util::string_t buf = lexStringLiteral(true);
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
                util::string_t buf = lexStringLiteral(false);
                if(!getError())
                {
                    Token t = createToken(TOKEN_LITERAL_STRING, buf);
                    t.modifierString = STRING_C;
                    return t;
                }
            }
            else
            {
                util::string_t buf;
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
            util::string_t buf;
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
                if(*(it + 1) == 'x' || *(it + 1) == 'X')
                {
                    _next();
                    currentChar = _getNext();
                    base = BASE_HEX;
                }
                else if(*(it + 1) == 'o' || *(it + 1) == 'O')
                {
                    _next();
                    currentChar = _getNext();
                    base = BASE_OCT;
                }
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
                    util::string_t modbuf;
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
                        if(advance() == end)
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
                util::string_t modbuf;
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
            util::string_t buf = lexStringLiteral(false);
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
            util::string_t buf = lexStringLiteral(true);
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
            util::string_t buf;
            while(util::stringutils::isCharPunctuation(currentChar))
            {
                if(getTokenTypeFromOperator(buf) != TOKEN_UNDEFINED)
                {
                    util::string_t tmp(buf);
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
                lexerError("Invalid operator or punctuator: '{}'", buf);
            }
            return t;
        }

        lexerWarning("Unrecognized token: '{}' ({})", currentChar,
                     static_cast<int32_t>(currentChar));
        advance();
        return createToken(TOKEN_DEFAULT,
                           util::stringutils::charToString(currentChar));
    }

    Lexer::LexCommentReturn Lexer::lexComment()
    {
        if(*it == '/')
        {
            // Single line comment: '//'
            if(peekNext() == '/')
            {
                util::logger->trace("Single-line comment");
                advance(); // Skip the both slashes
                advance();

                if(it == end)
                {
                    return COMMENT_EOF;
                }

                while(true)
                {
                    auto a = _advance();
                    if(a == 1)
                    {
                        util::logger->trace(
                            "Single-line comment ended with line break");
                        break;
                    }
                    if(a == 2)
                    {
                        util::logger->trace(
                            "Single-line comment ended with EOF");
                        return COMMENT_EOF;
                    }
                }
                return COMMENT_FOUND;
            }
            // Multi line comment: '/*'
            else if(peekNext() == '*')
            {
                advance(); // Skip '/'
                advance(); // Skip '*'

                int openCommentCount = 1;
                while(openCommentCount > 0)
                {
                    if(it == end)
                    {
                        lexerWarning("Unclosed multi-line comment");
                        return COMMENT_EOF;
                    }
                    if(*it == '/' && peekNext() == '*')
                    {
                        advance(); // Skip '/'
                        advance(); // Skip '*'
                        ++openCommentCount;
                        continue;
                    }
                    if(*it == '*' && peekNext() == '/')
                    {
                        advance(); // Skip '*'
                        advance(); // Skip '/'
                        --openCommentCount;
                        continue;
                    }
                    advance();
                }
                return COMMENT_FOUND;
            }
        }
        return COMMENT_NONE;
    }

    util::string_t Lexer::lexStringLiteral(bool isChar)
    {
        util::string_t buf;
        buf.reserve(isChar ? 2 : 8);
        const util::char_t quote = (isChar ? '\'' : '"');
        while(advance() != end)
        {
            util::char_t currentChar = *it;
            util::char_t prev = peekPrevious();
            util::char_t next = peekNext();
            util::logger->trace(
                "Current character: '{}', prev: '{}', next: '{}'", currentChar,
                prev, next);

            // Current char is a newline
            // String doesn't end, terminate
            if(currentChar == '\n')
            {
                lexerError("Invalid {} literal: Closing quote not found",
                           (isChar ? "character" : "string"));
                break;
            }
            // Current char is a quotation mark
            if(currentChar == quote)
            {
                if(prev == '\\' && peekPassed(2) != '\\')
                {
                    // Remove the backslash
                    buf.pop_back();
                }
                // Not escaped, end string
                else
                {
                    break;
                }
            }

            buf.push_back(currentChar);
        }

        util::logger->trace("Buffer before escaping: '{}'", buf);
        util::string_t escaped;
        std::string::const_iterator rit = buf.begin();
        while(rit != buf.end())
        {
            char c = *rit++;
            if(c == '\\' && rit != buf.end())
            {
                switch(*rit++)
                {
                case '\\':
                    c = '\\';
                    break;
                case 'n':
                    c = '\n';
                    break;
                case 't':
                    c = '\t';
                    break;
                case 'r':
                    c = '\r';
                    break;
                case 'f':
                    c = '\f';
                    break;
                case 'v':
                    c = '\v';
                    break;
                case 'b':
                    c = '\b';
                    break;
                case 'a':
                    c = '\a';
                    break;
                case 'x':
                {
                    util::string_t hex;
                    if(!util::stringutils::isCharHexDigit(*rit))
                    {
                        lexerWarning("Invalid hexadecimal escape sequence: The "
                                     "first character after an \\x should be a "
                                     "number in hexadecimal, got '{}' instead",
                                     *rit);
                        continue;
                    }
                    hex.push_back(*rit);
                    if(util::stringutils::isCharHexDigit(*(rit + 1)))
                    {
                        ++rit;
                        hex.push_back(*rit);
                    }
                    ++rit;
                    c = static_cast<char>(std::stol(hex, nullptr, 16));
                    break;
                }
                case 'o':
                {
                    util::string_t oct;
                    if(!util::stringutils::isCharOctDigit(*rit))
                    {
                        lexerWarning("Invalid octal escape sequence: The first "
                                     "character after an \\o should be a "
                                     "number in octal, got '{}' instead",
                                     *rit);
                        continue;
                    }
                    oct.push_back(*rit);
                    if(util::stringutils::isCharOctDigit(*(rit + 1)))
                    {
                        ++rit;
                        oct.push_back(*rit);
                        if(util::stringutils::isCharOctDigit(*(rit + 1)))
                        {
                            ++rit;
                            oct.push_back(*rit);
                        }
                        ++rit;
                    }
                    else
                    {
                        ++rit;
                    }
                    c = static_cast<char>(std::stol(oct, nullptr, 8));
                    break;
                }
                case '"':
                    if(!isChar)
                    {
                        c = '"';
                    }
                    break;
                case '\'':
                    if(isChar)
                    {
                        c = '\'';
                    }
                    break;
                default:
                    lexerWarning("Invalid escape sequence");
                    continue;
                }
            }
            escaped.push_back(c);
        }

        advance();
        return escaped;
    }

    Token Lexer::createToken(TokenType type, const util::string_t& val) const
    {
        return Token::create(type, val, currentLocation);
    }

    TokenType Lexer::getTokenTypeFromWord(const util::string_t& buf) const
    {
        static const std::unordered_map<util::string_t, TokenType> words{
            {"import", TOKEN_KEYWORD_IMPORT},
            {"export", TOKEN_KEYWORD_EXPORT},
            {"module", TOKEN_KEYWORD_MODULE},
            {"package", TOKEN_KEYWORD_PACKAGE},

            {"def", TOKEN_KEYWORD_DEFINE},
            {"if", TOKEN_KEYWORD_IF},
            {"else", TOKEN_KEYWORD_ELSE},
            {"while", TOKEN_KEYWORD_WHILE},
            {"for", TOKEN_KEYWORD_FOR},
            {"foreach", TOKEN_KEYWORD_FOREACH},
            {"return", TOKEN_KEYWORD_RETURN},
            {"cast", TOKEN_KEYWORD_CAST},
            {"use", TOKEN_KEYWORD_USE},

            {"let", TOKEN_KEYWORD_LET},
            {"mut", TOKEN_KEYWORD_MUT},

            {"true", TOKEN_LITERAL_TRUE},
            {"false", TOKEN_LITERAL_FALSE},

            {"and", TOKEN_OPERATORB_AND},
            {"or", TOKEN_OPERATORB_OR},
            {"not", TOKEN_OPERATORU_NOT},
            {"rem", TOKEN_OPERATORB_REM},
            {"as", TOKEN_OPERATORB_AS}};

        auto find = words.find(buf);
        if(find == words.end())
        {
            return TOKEN_IDENTIFIER;
        }
        return find->second;
    }

    Token Lexer::getTokenFromWord(const util::string_t& buf) const
    {
        return createToken(getTokenTypeFromWord(buf), buf);
    }

    TokenType Lexer::getTokenTypeFromOperator(const util::string_t& buf) const
    {
        static const std::unordered_map<util::string_t, TokenType> operators{
            {"=", TOKEN_OPERATORA_SIMPLE},  {"+=", TOKEN_OPERATORA_ADD},
            {"-=", TOKEN_OPERATORA_SUB},    {"*=", TOKEN_OPERATORA_MUL},
            {"/=", TOKEN_OPERATORA_DIV},    {"%=", TOKEN_OPERATORA_MOD},

            {"+", TOKEN_OPERATORB_ADD},     {"-", TOKEN_OPERATORB_SUB},
            {"*", TOKEN_OPERATORB_MUL},     {"/", TOKEN_OPERATORB_DIV},
            {"%", TOKEN_OPERATORB_MOD},

            {"&&", TOKEN_OPERATORB_AND},    {"||", TOKEN_OPERATORB_OR},

            {"==", TOKEN_OPERATORB_EQ},     {"!=", TOKEN_OPERATORB_NOTEQ},
            {"<", TOKEN_OPERATORB_LESS},    {">", TOKEN_OPERATORB_GREATER},
            {"<=", TOKEN_OPERATORB_LESSEQ}, {">=", TOKEN_OPERATORB_GREATEQ},

            {".", TOKEN_OPERATORB_MEMBER},  {"!", TOKEN_OPERATORU_NOT},

            {"(", TOKEN_PUNCT_PAREN_OPEN},  {")", TOKEN_PUNCT_PAREN_CLOSE},
            {"{", TOKEN_PUNCT_BRACE_OPEN},  {"}", TOKEN_PUNCT_BRACE_CLOSE},
            {"[", TOKEN_PUNCT_SQR_OPEN},    {"]", TOKEN_PUNCT_SQR_CLOSE},
            {":", TOKEN_PUNCT_COLON},       {";", TOKEN_PUNCT_SEMICOLON},
            {",", TOKEN_PUNCT_COMMA},       {"->", TOKEN_PUNCT_ARROW}};

        auto find = operators.find(buf);
        if(find == operators.end())
        {
            return TOKEN_UNDEFINED;
        }
        return find->second;
    }

    Token Lexer::getTokenFromOperator(const util::string_t& buf) const
    {
        return createToken(getTokenTypeFromOperator(buf), buf);
    }
} // namespace lexer
} // namespace core
