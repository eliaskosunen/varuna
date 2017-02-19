// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#pragma once

#include "util/OperatorType.h"
#include "util/SafeEnum.h"
#include <limits>

namespace core
{
namespace lexer
{
    using TokenTypeUnderlying = int32_t;

    enum _TokenType
    {
        TOKEN_UNDEFINED = -1,
        TOKEN_DEFAULT = 0,

        TOKEN_KEYWORD_IMPORT = 1,
        TOKEN_KEYWORD_EXPORT,
        TOKEN_KEYWORD_DEFINE,
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
        TOKEN_KEYWORD_LET,
        TOKEN_KEYWORD_MUT,
        TOKEN_KEYWORD_CAST,
        TOKEN_KEYWORD_USE,

        TOKEN_IDENTIFIER = 200,

        TOKEN_LITERAL_INTEGER = 300,
        TOKEN_LITERAL_FLOAT,
        TOKEN_LITERAL_STRING,
        TOKEN_LITERAL_CHAR,
        TOKEN_LITERAL_TRUE,
        TOKEN_LITERAL_FALSE,
        TOKEN_LITERAL_NONE,

        TOKEN_OPERATORA_SIMPLE = util::OPERATORA_SIMPLE, // =
        TOKEN_OPERATORA_ADD = util::OPERATORA_ADD,       // +=
        TOKEN_OPERATORA_SUB = util::OPERATORA_SUB,       // -=
        TOKEN_OPERATORA_MUL = util::OPERATORA_MUL,       // *=
        TOKEN_OPERATORA_DIV = util::OPERATORA_DIV,       // /=
        TOKEN_OPERATORA_MOD = util::OPERATORA_MOD,       // %=
        TOKEN_OPERATORA_POW = util::OPERATORA_POW,       // ^=
        TOKEN_OPERATORA_MOVE = util::OPERATORA_MOVE,     // <-

        TOKEN_OPERATORB_ADD = util::OPERATORB_ADD, // a + b
        TOKEN_OPERATORB_SUB = util::OPERATORB_SUB, // a - b
        TOKEN_OPERATORB_MUL = util::OPERATORB_MUL, // a * b
        TOKEN_OPERATORB_DIV = util::OPERATORB_DIV, // a / b
        TOKEN_OPERATORB_MOD = util::OPERATORB_MOD, // a % b, modulo
        TOKEN_OPERATORB_REM = util::OPERATORB_REM, // a rem b, remainder
        TOKEN_OPERATORB_POW = util::OPERATORB_POW, // a ^ b, power

        TOKEN_OPERATORB_AND = util::OPERATORB_AND, // a && b, a and b
        TOKEN_OPERATORB_OR = util::OPERATORB_OR,   // a || b, a or b

        TOKEN_OPERATORB_EQ = util::OPERATORB_EQ,           // a == b
        TOKEN_OPERATORB_NOTEQ = util::OPERATORB_NOTEQ,     // a != b
        TOKEN_OPERATORB_LESS = util::OPERATORB_LESS,       // a < b
        TOKEN_OPERATORB_GREATER = util::OPERATORB_GREATER, // a > b
        TOKEN_OPERATORB_LESSEQ = util::OPERATORB_LESSEQ,   // a <= b
        TOKEN_OPERATORB_GREATEQ = util::OPERATORB_GREATEQ, // a >= b

        TOKEN_OPERATORB_MEMBER = util::OPERATORB_MEMBER, // a.b

        TOKEN_OPERATORB_OF = util::OPERATORB_OF, // List of String
        TOKEN_OPERATORB_AS = util::OPERATORB_AS, // foreach(list as String elem)
        TOKEN_OPERATORB_INSTOF = util::OPERATORB_INSTOF, // a instanceof T

        TOKEN_OPERATORU_INC = util::OPERATORU_INC, // a++
        TOKEN_OPERATORU_DEC = util::OPERATORU_DEC, // a--

        TOKEN_OPERATORU_REF = util::OPERATORU_REF, // &a

        TOKEN_OPERATORU_PLUS = util::OPERATORU_PLUS,
        TOKEN_OPERATORU_MINUS = util::OPERATORU_MINUS,
        TOKEN_OPERATORU_NOT = util::OPERATORU_NOT, // !a, not a

        TOKEN_OPERATORU_SIZEOF = util::OPERATORU_SIZEOF, // sizeof a
        TOKEN_OPERATORU_TYPEOF = util::OPERATORU_TYPEOF, // typeof a
        TOKEN_OPERATORU_ADDROF = util::OPERATORU_ADDROF, // addressof a

        TOKEN_OPERATORC_CALL = util::OPERATORC_CALL, // ()

        TOKEN_PUNCT_PAREN_OPEN = util::PUNCT_PAREN_OPEN,   // (
        TOKEN_PUNCT_PAREN_CLOSE = util::PUNCT_PAREN_CLOSE, // )
        TOKEN_PUNCT_BRACE_OPEN = util::PUNCT_BRACE_OPEN,   // {
        TOKEN_PUNCT_BRACE_CLOSE = util::PUNCT_BRACE_CLOSE, // }
        TOKEN_PUNCT_SQR_OPEN = util::PUNCT_SQR_OPEN,       // [
        TOKEN_PUNCT_SQR_CLOSE = util::PUNCT_SQR_CLOSE,     // ]
        TOKEN_PUNCT_COLON = util::PUNCT_COLON,             // :
        TOKEN_PUNCT_SEMICOLON = util::PUNCT_SEMICOLON,     // ;
        TOKEN_PUNCT_COMMA = util::PUNCT_COMMA,             // ,
        TOKEN_PUNCT_ARROW = util::PUNCT_ARROW,             // ->

        TOKEN_EOF = std::numeric_limits<TokenTypeUnderlying>::max()
    };

    using TokenType = util::SafeEnum<_TokenType, TokenTypeUnderlying>;
} // namespace lexer
} // namespace core
