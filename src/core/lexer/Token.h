// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#pragma once

#include "core/lexer/TokenType.h"
#include "util/Logger.h"
#include "util/SafeEnum.h"
#include "util/SourceLocation.h"
#include <map>
#include <string>

namespace core
{
namespace lexer
{
    enum _TokenIntegerLiteralModifier
    {
        INTEGER_INT32 = 1, // none, i32
        INTEGER_INT8 = 2,  // i8
        INTEGER_INT16 = 4, // i16
        INTEGER_INT64 = 8, // i64
        INTEGER_BYTE = 16, // b
        INTEGER_BIN = 32,  // 0b...
        INTEGER_OCT = 64,  // 0o...
        INTEGER_HEX = 128, // 0x...

        INTEGER_NONE = 0
    };

    enum _TokenFloatLiteralModifier
    {
        FLOAT_F64 = 1,     // f64
        FLOAT_F32 = 2,     // f32
        FLOAT_DECIMAL = 4, // d
        FLOAT_LONG = 8,    // l

        FLOAT_NONE = 0
    };

    enum _TokenCharLiteralModifier
    {
        CHAR_CHAR = 1, // none
        CHAR_BYTE = 2, // b...

        CHAR_NONE = 0
    };

    enum _TokenStringLiteralModifier
    {
        STRING_STRING = 1, // none
        STRING_C = 2,      // c...

        STRING_NONE = 0
    };

    using TokenIntegerLiteralModifier =
        util::SafeEnum<_TokenIntegerLiteralModifier>;
    using TokenFloatLiteralModifier =
        util::SafeEnum<_TokenFloatLiteralModifier>;
    using TokenCharLiteralModifier = util::SafeEnum<_TokenCharLiteralModifier>;
    using TokenStringLiteralModifier =
        util::SafeEnum<_TokenStringLiteralModifier>;

    struct Token final
    {
        explicit Token(util::SourceLocation l, TokenType t = TOKEN_DEFAULT,
                       std::string val = "")
            : loc(std::move(l)), type(t), value(std::move(val)),
              modifierInt(INTEGER_NONE), modifierFloat(FLOAT_NONE),
              modifierChar(CHAR_NONE), modifierString(STRING_NONE)
        {
        }

        std::string typeToString() const;

        util::SourceLocation loc;

        TokenType type;
        std::string value;

        TokenIntegerLiteralModifier modifierInt;
        TokenFloatLiteralModifier modifierFloat;
        TokenCharLiteralModifier modifierChar;
        TokenStringLiteralModifier modifierString;
    };
} // namespace lexer
} // namespace core
