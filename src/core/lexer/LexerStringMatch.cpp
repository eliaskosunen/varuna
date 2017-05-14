// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#include "core/lexer/Lexer.h"
#include "util/StringUtils.h"

namespace core
{
namespace lexer
{
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

                if(!it.hasMore())
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
            if(peekNext() == '*')
            {
                advance(); // Skip '/'
                advance(); // Skip '*'

                int openCommentCount = 1;
                while(openCommentCount > 0)
                {
                    if(!it.hasMore())
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

    std::string Lexer::lexStringLiteral(bool isChar)
    {
        std::string buf;
        buf.reserve(isChar ? 2 : 8);
        const char quote = (isChar ? '\'' : '"');
        while(advance().hasMore())
        {
            char currentChar = *it;
            char prev = peekPrevious();
            char next = peekNext();
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
        std::string escaped;
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
                    std::string hex;
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
                    std::string oct;
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

    TokenType Lexer::getTokenTypeFromWord(const std::string& buf) const
    {
        static const std::unordered_map<std::string, TokenType> words{
            {"import", TOKEN_KEYWORD_IMPORT},
            {"export", TOKEN_KEYWORD_EXPORT},
            {"module", TOKEN_KEYWORD_MODULE},
            {"package", TOKEN_KEYWORD_PACKAGE},
            {"nomangle", TOKEN_KEYWORD_NO_MANGLE},

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

    Token Lexer::getTokenFromWord(const std::string& buf) const
    {
        return createToken(getTokenTypeFromWord(buf), buf);
    }

    TokenType Lexer::getTokenTypeFromOperator(const std::string& buf) const
    {
        static const std::unordered_map<std::string, TokenType> operators{
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

    Token Lexer::getTokenFromOperator(const std::string& buf) const
    {
        return createToken(getTokenTypeFromOperator(buf), buf);
    }
} // namespace lexer
} // namespace core
