// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#include "core/lexer/Token.h"
#include <string>

namespace core
{
namespace lexer
{
    std::string Token::typeToString() const
    {
        switch(type.get())
        {
        case TOKEN_KEYWORD_IMPORT:
            return "KEYWORD_IMPORT";
        case TOKEN_IDENTIFIER:
            return "IDENTIFIER";

        case TOKEN_LITERAL_INTEGER:
            return "LITERAL_INTEGER";
        case TOKEN_LITERAL_FLOAT:
            return "LITERAL_FLOAT";
        case TOKEN_LITERAL_STRING:
            return "LITERAL_STRING";
        case TOKEN_LITERAL_TRUE:
            return "LITERAL_TRUE";
        case TOKEN_LITERAL_FALSE:
            return "LITERAL_FALSE";

        case TOKEN_OPERATORB_ADD:
            return "OPERATORB_ADD";
        case TOKEN_OPERATORB_MUL:
            return "OPERATORB_MUL";

        case TOKEN_EOF:
            return "EOF";
        case TOKEN_DEFAULT:
            return "DEFAULT";
        default:
            return "[unknown]";
        }
    }
} // namespace lexer
} // namespace core
