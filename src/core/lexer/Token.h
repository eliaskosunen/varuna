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

#pragma once

#include "core/lexer/TokenType.h"
#include "util/Logger.h"
#include "util/SafeEnum.h"
#include <map>
#include <string>

namespace core
{
namespace lexer
{
    enum TokenIntegerLiteralModifier_t
    {
        INTEGER_INT = 1,     // none
        INTEGER_INT8 = 2,    // i8
        INTEGER_INT16 = 4,   // i16
        INTEGER_INT32 = 8,   // i32
        INTEGER_INT64 = 16,  // i64
        INTEGER_BINARY = 32, // b
        INTEGER_OCTAL = 64,  // o
        INTEGER_HEX = 128,   // x

        INTEGER_NONE = 0
    };

    enum TokenFloatLiteralModifier_t
    {
        FLOAT_FLOAT = 1,   // none
        FLOAT_F64 = 2,     // f64
        FLOAT_F32 = 4,     // f32
        FLOAT_DECIMAL = 8, // d

        FLOAT_NONE = 0
    };

    using TokenIntegerLiteralModifier =
        util::SafeEnum<TokenIntegerLiteralModifier_t>;
    using TokenFloatLiteralModifier =
        util::SafeEnum<TokenFloatLiteralModifier_t>;

    class SourceLocation
    {
    public:
        std::string file{"undefined"};
        uint64_t line{0};

        SourceLocation() = default;
        SourceLocation(std::string f, uint64_t l) : file(std::move(f)), line(l)
        {
        }

        std::string toString() const
        {
            // return fmt::format("{}:{}:{}", file, line, column);
            return file + ":" + std::to_string(line);
        }
    };

    class Token final
    {
    public:
        SourceLocation loc;

        TokenType type;
        std::string value;

        TokenIntegerLiteralModifier modifierInt;
        TokenFloatLiteralModifier modifierFloat;

        Token(TokenType t = TOKEN_DEFAULT, std::string val = "")
            : loc(), type(t), value(std::move(val)), modifierInt(INTEGER_NONE),
              modifierFloat(FLOAT_NONE)
        {
        }

        std::string typeToString() const;

        static std::string typeToString(TokenType t)
        {
            Token tok(t, "");
            return tok.typeToString();
        }

        static Token create(TokenType t, const std::string& val,
                            SourceLocation loc = SourceLocation())
        {
            Token tok(t, val);
            tok.loc = loc;
            return tok;
        }
    };
} // namespace lexer
} // namespace core
