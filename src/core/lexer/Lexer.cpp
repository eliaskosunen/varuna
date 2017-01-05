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
    Token Lexer::getNextToken()
    {
        util::char_t currentChar = *it;
        util::loggerBasic->trace("");

        if(it == end)
        {
            return createToken(TOKEN_EOF, "EOF");
        }

        if(!lexComment())
        {
            return createToken(TOKEN_EOF, "EOF");
        }
        currentChar = *it;

        util::logger->trace(
            "Getting next token. Current character: '{}', on {}",
            std::to_string(currentChar), currentLocation.toString());

        // Skip any whitespace
        while(util::StringUtils::isCharWhitespace(currentChar))
        {
            util::logger->trace("Skipping whitespace");
            currentChar = *advance();

            if(it == end)
            {
                return createToken(TOKEN_EOF, "EOF");
            }
            return getNextToken();
        }

        if(!lexComment())
        {
            return createToken(TOKEN_EOF, "EOF");
        }
        currentChar = *it;
        /*
                    if(currentChar == '\0')
                    {
                        while(true)
                        {
                            currentChar = *advance();
                            if(it == end)
                            {
                                return createToken(TOKEN_EOF, "EOF");
                            }
                            else if(currentChar == '\0')
                            {
                                continue;
                            }
                            else
                            {
                                break;
                            }
                        }
                    }
        */
        // Invalid character
        if(util::StringUtils::isCharControlCharacter(currentChar) &&
           !util::StringUtils::isCharWhitespace(currentChar))
        {
            fmt::MemoryWriter out;
            out << "'" << currentChar
                << "', dec: " << util::StringUtils::charToString(currentChar)
                << " hex: " << fmt::hex(currentChar);
            lexerWarning("Unrecognized character: {}, skipped", out.str());
            advance();
            return createToken(TOKEN_DEFAULT,
                               util::StringUtils::charToString(currentChar));
        }

        // Word: a keyword or an identifier
        // [a-zA-Z_][a-zA-Z_0-9]*
        if(util::StringUtils::isCharAlpha(currentChar) || currentChar == '_')
        {
            util::string_t buf;
            buf.reserve(8);
            buf.push_back(currentChar);
            while(util::StringUtils::isCharAlnum(*advance()) || *it == '_')
            {
                buf.push_back(*it);
            }

            // Identify the meaning of the word
            return getTokenFromWord(buf);
        }

        // Number literal
        // -?[0-9]([0-9\.]*)[dfulsboh]
        if(util::StringUtils::isCharDigit(currentChar) ||
           (currentChar == '.' && util::StringUtils::isCharDigit(*(it + 1))))
        {
            util::string_t buf;
            buf.reserve(8);
            bool isFloatingPoint = (currentChar == '.');
            bool isHex = false;
            if(currentChar == '0' && *(it + 1) == 'x')
            {
                currentChar = *(it += 2);
                isHex = true;
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
                cont = (isHex ? util::StringUtils::isCharHexDigit(currentChar)
                              : util::StringUtils::isCharDigit(currentChar));
            } while(cont || currentChar == '.');

            if(!isFloatingPoint)
            {
                Token t = createToken(TOKEN_LITERAL_INTEGER, buf);
                std::unordered_map<std::string, decltype(INTEGER_INT)>
                    allowedModifiers = {
                        {"i64", INTEGER_INT64}, {"i32", INTEGER_INT32},
                        {"i16", INTEGER_INT16}, {"i8", INTEGER_INT8},
                        {"b", INTEGER_BINARY},  {"o", INTEGER_OCTAL}};
                auto mod = [&]() {
                    util::string_t modbuf;
                    while(util::StringUtils::isCharAlnum(currentChar))
                    {
                        modbuf.push_back(currentChar);
                        auto modit = allowedModifiers.find(modbuf);
                        if(modit != allowedModifiers.end())
                        {
                            t.modifierInt |= modit->second;
                            allowedModifiers.erase(modit);
                            modbuf.clear();
                        }
                        currentChar = *advance();
                    }
                    return modbuf;
                }();
                if(!mod.empty())
                {
                    lexerError("Invalid integer suffix: '{}'", mod);
                }
                if(t.modifierInt == INTEGER_NONE)
                {
                    t.modifierInt |= INTEGER_INT;
                }
                if(isHex)
                {
                    t.modifierInt |= INTEGER_HEX;
                }
                return t;
            }

            Token t = createToken(TOKEN_LITERAL_FLOAT, buf);
            std::unordered_map<std::string, decltype(FLOAT_FLOAT)>
                allowedModifiers = {
                    {"f32", FLOAT_F32},
                    {"f64", FLOAT_F64},
                    {"d", FLOAT_DECIMAL},
                };
            auto mod = [&]() {
                util::string_t modbuf;
                while(util::StringUtils::isCharAlnum(currentChar))
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
                t.modifierFloat |= FLOAT_FLOAT;
            }
            if(isHex)
            {
                lexerError("Floating-point number cannot be in hexadecimal");
            }
            return t;
        }

        // String literal
        // ".+"
        if(currentChar == '"')
        {
            util::string_t buf = lexStringLiteral();
            if(!getError())
            {
                return createToken(TOKEN_LITERAL_STRING, buf);
            }
        }

        // Character literal
        // '.'
        if(currentChar == '\'')
        {
            util::string_t buf = lexStringLiteral(true);
            if(!getError())
            {
                return createToken(TOKEN_LITERAL_CHAR, buf);
            }
        }

        // Operator or punctuator
        if(util::StringUtils::isCharPunctuation(currentChar))
        {
            util::string_t buf;
            while(util::StringUtils::isCharPunctuation(currentChar))
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
                currentChar = *advance();
            }
            Token t = getTokenFromOperator(buf);
            if(t.type == TOKEN_UNDEFINED)
            {
                lexerError("Invalid operator or punctuator: '{}'", buf);
            }
            return t;
        }

        fmt::MemoryWriter out;
        out << "'" << currentChar
            << "', dec: " << util::StringUtils::charToString(currentChar)
            << " hex: " << fmt::hex(currentChar);
        lexerWarning("Unrecognized token: {}", out.str());
        advance();
        return createToken(TOKEN_DEFAULT,
                           util::StringUtils::charToString(currentChar));
    }

    TokenVector Lexer::run()
    {
        // Initialize variables
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
        if(tokens.size() == 1 && tokens[0].type == TOKEN_EOF)
        {
            lexerWarning("Empty translation unit");
        }
        if(tokens.back().type != TOKEN_EOF)
        {
            lexerError("No EOF token found");
        }
        return tokens;
    }

    bool Lexer::lexComment()
    {
        if(*it == '/' && peekNext() == '/')
        {
            advance(); // Skip the both slashes
            advance();
            do
            {
                if(it == end)
                {
                    return false;
                }
            } while(*advance() != '\n');
            advance(); // Skip '\n'
        }
        return true;
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
                "Current character: '{}', prev: '{}', next: '{}'",
                std::to_string(currentChar), prev, next);

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
                    if(!util::StringUtils::isCharHexDigit(*rit))
                    {
                        lexerWarning("Invalid hexadecimal escape sequence: The "
                                     "first character after an \\x should be a "
                                     "number in hexadecimal, got '{}' instead",
                                     *rit);
                        continue;
                    }
                    hex.push_back(*rit);
                    if(util::StringUtils::isCharHexDigit(*(rit + 1)))
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
                    if(!util::StringUtils::isCharOctDigit(*rit))
                    {
                        lexerWarning("Invalid octal escape sequence: The first "
                                     "character after an \\o should be a "
                                     "number in octal, got '{}' instead",
                                     *rit);
                        continue;
                    }
                    oct.push_back(*rit);
                    if(util::StringUtils::isCharOctDigit(*(rit + 1)))
                    {
                        ++rit;
                        oct.push_back(*rit);
                        if(util::StringUtils::isCharOctDigit(*(rit + 1)))
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

        if(isChar && escaped.length() > 1)
        {
            lexerError("Invalid character literal: Length more than 1: '{}'",
                       escaped);
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
        // clang-format off
        // Keywords
        if(buf == "import") return TOKEN_KEYWORD_IMPORT;
        if(buf == "def") return TOKEN_KEYWORD_DEFINE;
        if(buf == "decl") return TOKEN_KEYWORD_DECLARE;
        if(buf == "class") return TOKEN_KEYWORD_CLASS;
        if(buf == "override") return TOKEN_KEYWORD_OVERRIDE;
        if(buf == "final") return TOKEN_KEYWORD_FINAL;
        if(buf == "extend") return TOKEN_KEYWORD_EXTEND;
        if(buf == "abstract") return TOKEN_KEYWORD_ABSTRACT;
        if(buf == "implement") return TOKEN_KEYWORD_IMPLEMENT;
        if(buf == "interface") return TOKEN_KEYWORD_INTERFACE;
        if(buf == "public") return TOKEN_KEYWORD_PUBLIC;
        if(buf == "protected") return TOKEN_KEYWORD_PROTECTED;
        if(buf == "private") return TOKEN_KEYWORD_PRIVATE;
        if(buf == "if") return TOKEN_KEYWORD_IF;
        if(buf == "else") return TOKEN_KEYWORD_ELSE;
        if(buf == "while") return TOKEN_KEYWORD_WHILE;
        if(buf == "for") return TOKEN_KEYWORD_FOR;
        if(buf == "foreach") return TOKEN_KEYWORD_FOREACH;
        if(buf == "switch") return TOKEN_KEYWORD_SWITCH;
        if(buf == "case") return TOKEN_KEYWORD_CASE;
        if(buf == "break") return TOKEN_KEYWORD_BREAK;
        if(buf == "return") return TOKEN_KEYWORD_RETURN;
        if(buf == "continue") return TOKEN_KEYWORD_CONTINUE;
        if(buf == "module") return TOKEN_KEYWORD_MODULE;
        if(buf == "package") return TOKEN_KEYWORD_PACKAGE;
        if(buf == "extern") return TOKEN_KEYWORD_EXTERN;
        if(buf == "readonly") return TOKEN_KEYWORD_READONLY;
        if(buf == "view") return TOKEN_KEYWORD_VIEW;
        if(buf == "ref") return TOKEN_KEYWORD_REF;
        if(buf == "var") return TOKEN_KEYWORD_VAR;
        if(buf == "let") return TOKEN_KEYWORD_LET;
        if(buf == "cast") return TOKEN_KEYWORD_CAST;

        // Literals
        if(buf == "true") return TOKEN_LITERAL_TRUE;
        if(buf == "false") return TOKEN_LITERAL_FALSE;
        if(buf == "none") return TOKEN_LITERAL_NONE;

        // Textual operators
        if(buf == "and") return TOKEN_OPERATORB_AND;
        if(buf == "or") return TOKEN_OPERATORB_OR;
        if(buf == "not") return TOKEN_OPERATORU_NOT;
        if(buf == "of") return TOKEN_OPERATORB_OF;
        if(buf == "as") return TOKEN_OPERATORB_AS;
        if(buf == "rem") return TOKEN_OPERATORB_REM;
        if(buf == "instanceof") return TOKEN_OPERATORB_INSTOF;
        if(buf == "sizeof") return TOKEN_OPERATORU_SIZEOF;
        if(buf == "typeof") return TOKEN_OPERATORU_TYPEOF;
        if(buf == "addressof") return TOKEN_OPERATORU_ADDROF;
        // clang-format on

        return TOKEN_IDENTIFIER;
    }

    Token Lexer::getTokenFromWord(const util::string_t& buf) const
    {
        return createToken(getTokenTypeFromWord(buf), buf);
    }

    TokenType Lexer::getTokenTypeFromOperator(const util::string_t& buf) const
    {
        // clang-format off
        if(buf == "=") return TOKEN_OPERATORA_SIMPLE;
        if(buf == "+=") return TOKEN_OPERATORA_ADD;
        if(buf == "-=") return TOKEN_OPERATORA_SUB;
        if(buf == "*=") return TOKEN_OPERATORA_MUL;
        if(buf == "/=") return TOKEN_OPERATORA_DIV;
        if(buf == "%=") return TOKEN_OPERATORA_MOD;
        if(buf == "^=") return TOKEN_OPERATORA_POW;
        if(buf == "<-") return TOKEN_OPERATORA_MOVE;

        if(buf == "+") return TOKEN_OPERATORB_ADD;
        if(buf == "-") return TOKEN_OPERATORB_SUB;
        if(buf == "*") return TOKEN_OPERATORB_MUL;
        if(buf == "/") return TOKEN_OPERATORB_DIV;
        if(buf == "%") return TOKEN_OPERATORB_MOD;
        if(buf == "^") return TOKEN_OPERATORB_POW;

        if(buf == "&&") return TOKEN_OPERATORB_AND;
        if(buf == "||") return TOKEN_OPERATORB_OR;

        if(buf == "==") return TOKEN_OPERATORB_EQ;
        if(buf == "!=") return TOKEN_OPERATORB_NOTEQ;
        if(buf == "<") return TOKEN_OPERATORB_LESS;
        if(buf == ">") return TOKEN_OPERATORB_GREATER;
        if(buf == "<=") return TOKEN_OPERATORB_LESSEQ;
        if(buf == ">=") return TOKEN_OPERATORB_GREATEQ;

        if(buf == ".") return TOKEN_OPERATORB_MEMBER;

        if(buf == "++") return TOKEN_OPERATORU_INC;
        if(buf == "--") return TOKEN_OPERATORU_DEC;

        if(buf == "&") return TOKEN_OPERATORU_REF;

        if(buf == "!") return TOKEN_OPERATORU_NOT;

        if(buf == "(") return TOKEN_PUNCT_PAREN_OPEN;
        if(buf == ")") return TOKEN_PUNCT_PAREN_CLOSE;
        if(buf == "{") return TOKEN_PUNCT_BRACE_OPEN;
        if(buf == "}") return TOKEN_PUNCT_BRACE_CLOSE;
        if(buf == "[") return TOKEN_PUNCT_SQR_OPEN;
        if(buf == "]") return TOKEN_PUNCT_SQR_CLOSE;
        if(buf == ":") return TOKEN_PUNCT_COLON;
        if(buf == ";") return TOKEN_PUNCT_SEMICOLON;
        if(buf == ",") return TOKEN_PUNCT_COMMA;
        // clang-format on

        return TOKEN_UNDEFINED;
    }

    Token Lexer::getTokenFromOperator(const util::string_t& buf) const
    {
        return createToken(getTokenTypeFromOperator(buf), buf);
    }
} // namespace lexer
} // namespace core
