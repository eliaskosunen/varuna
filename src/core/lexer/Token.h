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
        INTEGER_INT = 1,    // none
        INTEGER_INT8 = 2,   // i8
        INTEGER_INT16 = 4,  // i16
        INTEGER_INT32 = 8,  // i32
        INTEGER_INT64 = 16, // i64
        INTEGER_BYTE = 32,  // b
        INTEGER_BIN = 64,   // 0b...
        INTEGER_OCT = 128,  // 0o...
        INTEGER_HEX = 256,  // 0x...

        INTEGER_NONE = 0
    };

    enum _TokenFloatLiteralModifier
    {
        FLOAT_FLOAT = 1,   // none
        FLOAT_F64 = 2,     // f64
        FLOAT_F32 = 4,     // f32
        FLOAT_DECIMAL = 8, // d

        FLOAT_NONE = 0
    };

    enum _TokenCharLiteralModifier
    {
        CHAR_CHAR = 1, // none
        CHAR_BYTE = 2, // b...

        CHAR_NONE = 0
    };

    using TokenIntegerLiteralModifier =
        util::SafeEnum<_TokenIntegerLiteralModifier>;
    using TokenFloatLiteralModifier =
        util::SafeEnum<_TokenFloatLiteralModifier>;
    using TokenCharLiteralModifier = util::SafeEnum<_TokenCharLiteralModifier>;

    struct Token final
    {
        explicit Token(TokenType t = TOKEN_DEFAULT, std::string val = "")
            : loc(), type(t), value(std::move(val)), modifierInt(INTEGER_NONE),
              modifierFloat(FLOAT_NONE), modifierChar(CHAR_NONE)
        {
        }

        std::string typeToString() const;

        static std::string typeToString(TokenType t)
        {
            Token tok(t, "");
            return tok.typeToString();
        }

        static Token create(TokenType t, const std::string& val,
                            util::SourceLocation loc)
        {
            Token tok(t, val);
            tok.loc = loc;
            return tok;
        }

        util::SourceLocation loc;

        TokenType type;
        std::string value;

        TokenIntegerLiteralModifier modifierInt;
        TokenFloatLiteralModifier modifierFloat;
        TokenCharLiteralModifier modifierChar;
    };
} // namespace lexer
} // namespace core
