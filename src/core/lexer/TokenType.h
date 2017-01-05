// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#pragma once

#include "util/SafeEnum.h"

namespace core
{
namespace lexer
{
    enum TokenType_t
    {
        TOKEN_UNDEFINED = -1,
        TOKEN_DEFAULT = 0,

        TOKEN_KEYWORD_IMPORT = 1,
        TOKEN_KEYWORD_DEFINE,
        TOKEN_KEYWORD_DECLARE,
        TOKEN_KEYWORD_CLASS,
        TOKEN_KEYWORD_OVERRIDE,
        TOKEN_KEYWORD_FINAL,
        TOKEN_KEYWORD_EXTEND,
        TOKEN_KEYWORD_ABSTRACT,
        TOKEN_KEYWORD_IMPLEMENT,
        TOKEN_KEYWORD_INTERFACE,
        TOKEN_KEYWORD_PUBLIC,
        TOKEN_KEYWORD_PROTECTED,
        TOKEN_KEYWORD_PRIVATE,
        TOKEN_KEYWORD_IF,
        TOKEN_KEYWORD_ELSE,
        TOKEN_KEYWORD_WHILE,
        TOKEN_KEYWORD_FOR,
        TOKEN_KEYWORD_FOREACH,
        TOKEN_KEYWORD_SWITCH,
        TOKEN_KEYWORD_CASE,
        TOKEN_KEYWORD_BREAK,
        TOKEN_KEYWORD_RETURN,
        TOKEN_KEYWORD_CONTINUE,
        TOKEN_KEYWORD_MODULE,
        TOKEN_KEYWORD_PACKAGE,
        TOKEN_KEYWORD_EXTERN,
        TOKEN_KEYWORD_READONLY,
        TOKEN_KEYWORD_VIEW,
        TOKEN_KEYWORD_REF,
        TOKEN_KEYWORD_VAR,
        TOKEN_KEYWORD_LET,
        TOKEN_KEYWORD_CAST,

        TOKEN_IDENTIFIER = 200,

        TOKEN_LITERAL_INTEGER = 300,
        TOKEN_LITERAL_FLOAT,
        TOKEN_LITERAL_STRING,
        TOKEN_LITERAL_CHAR,
        TOKEN_LITERAL_TRUE,
        TOKEN_LITERAL_FALSE,
        TOKEN_LITERAL_NONE,

        TOKEN_OPERATORA_SIMPLE = 400, // =
        TOKEN_OPERATORA_ADD,          // +=
        TOKEN_OPERATORA_SUB,          // -=
        TOKEN_OPERATORA_MUL,          // *=
        TOKEN_OPERATORA_DIV,          // /=
        TOKEN_OPERATORA_MOD,          // %=
        TOKEN_OPERATORA_POW,          // ^=
        TOKEN_OPERATORA_MOVE,         // <-

        TOKEN_OPERATORB_ADD = 500, // a + b
        TOKEN_OPERATORB_SUB,       // a - b
        TOKEN_OPERATORB_MUL,       // a * b
        TOKEN_OPERATORB_DIV,       // a / b
        TOKEN_OPERATORB_MOD,       // a % b, modulo
        TOKEN_OPERATORB_REM,       // a rem b, remainder
        TOKEN_OPERATORB_POW,       // a ^ b, power

        TOKEN_OPERATORB_AND, // a && b, a and b
        TOKEN_OPERATORB_OR,  // a || b, a or b

        TOKEN_OPERATORB_EQ,      // a == b
        TOKEN_OPERATORB_NOTEQ,   // a != b
        TOKEN_OPERATORB_LESS,    // a < b
        TOKEN_OPERATORB_GREATER, // a > b
        TOKEN_OPERATORB_LESSEQ,  // a <= b
        TOKEN_OPERATORB_GREATEQ, // a >= b

        TOKEN_OPERATORB_MEMBER, // a.b

        TOKEN_OPERATORB_OF,     // List of String
        TOKEN_OPERATORB_AS,     // foreach(list as String elem)
        TOKEN_OPERATORB_INSTOF, // a instanceof T

        TOKEN_OPERATORU_INC = 600, // a++
        TOKEN_OPERATORU_DEC,       // a--

        TOKEN_OPERATORU_REF, // &a

        TOKEN_OPERATORU_PLUS,
        TOKEN_OPERATORU_MINUS,
        TOKEN_OPERATORU_NOT, // !a, not a

        TOKEN_OPERATORU_SIZEOF, // sizeof a
        TOKEN_OPERATORU_TYPEOF, // typeof a
        TOKEN_OPERATORU_ADDROF, // addressof a

        TOKEN_PUNCT_PAREN_OPEN = 700, // (
        TOKEN_PUNCT_PAREN_CLOSE,      // )
        TOKEN_PUNCT_BRACE_OPEN,       // {
        TOKEN_PUNCT_BRACE_CLOSE,      // }
        TOKEN_PUNCT_SQR_OPEN,         // [
        TOKEN_PUNCT_SQR_CLOSE,        // ]
        TOKEN_PUNCT_COLON,            // :
        TOKEN_PUNCT_SEMICOLON,        // ;
        TOKEN_PUNCT_COMMA,            // ,

        TOKEN_EOF = 32767
    };

    using TokenType = util::SafeEnum<TokenType_t>;
} // namespace lexer
} // namespace core
