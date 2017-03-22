// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#pragma once

#include "util/SafeEnum.h"

namespace util
{
enum _OperatorType
{
    OPERATOR_DEFAULT = -1,

    OPERATORA_SIMPLE = 400, // =
    OPERATORA_ADD,          // +=
    OPERATORA_SUB,          // -=
    OPERATORA_MUL,          // *=
    OPERATORA_DIV,          // /=
    OPERATORA_MOD,          // %=
    OPERATORA_POW,          // ^=
    OPERATORA_MOVE,         // <-

    OPERATORB_ADD = 500, // a + b
    OPERATORB_SUB,       // a - b
    OPERATORB_MUL,       // a * b
    OPERATORB_DIV,       // a / b
    OPERATORB_MOD,       // a % b, modulo
    OPERATORB_REM,       // a rem b, remainder
    OPERATORB_POW,       // a ^ b, power

    OPERATORB_AND, // a && b, a and b
    OPERATORB_OR,  // a || b, a or b

    OPERATORB_EQ,      // a == b
    OPERATORB_NOTEQ,   // a != b
    OPERATORB_LESS,    // a < b
    OPERATORB_GREATER, // a > b
    OPERATORB_LESSEQ,  // a <= b
    OPERATORB_GREATEQ, // a >= b

    OPERATORB_MEMBER, // a.b
    OPERATORB_SUBSCR, // a[b]

    OPERATORB_OF,     // List of String
    OPERATORB_AS,     // 12 as i16
    OPERATORB_INSTOF, // a instanceof T

    OPERATORU_INC = 600, // a++
    OPERATORU_DEC,       // a--

    OPERATORU_REF, // &a

    OPERATORU_PLUS,
    OPERATORU_MINUS,
    OPERATORU_NOT, // !a, not a

    OPERATORU_SIZEOF, // sizeof a
    OPERATORU_TYPEOF, // typeof a
    OPERATORU_ADDROF, // addressof a

    OPERATORC_CALL = 700, // ()

    PUNCT_PAREN_OPEN = 800, // (
    PUNCT_PAREN_CLOSE,      // )
    PUNCT_BRACE_OPEN,       // {
    PUNCT_BRACE_CLOSE,      // }
    PUNCT_SQR_OPEN,         // [
    PUNCT_SQR_CLOSE,        // ]
    PUNCT_COLON,            // :
    PUNCT_SEMICOLON,        // ;
    PUNCT_COMMA,            // ,
    PUNCT_ARROW             // ->
};

using OperatorType = util::SafeEnum<_OperatorType>;
}
