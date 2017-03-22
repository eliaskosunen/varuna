// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#pragma once

#include "codegen/Type.h"
#include <llvm/IR/Value.h>

namespace codegen
{
struct TypedValue
{
    enum ValueCategory
    {
        STMTVALUE = 0,
        LVALUE = 1 << 0,
        RVALUE = 1 << 1
    };

    TypedValue(Type* t, llvm::Value* v, ValueCategory c, bool mut)
        : type(t), value(v), cat(c), isMutable(mut)
    {
    }

    std::unique_ptr<TypedValue> clone() const
    {
        return std::make_unique<TypedValue>(type, value, cat, isMutable);
    }

    Type* type;
    llvm::Value* value;
    ValueCategory cat;
    bool isMutable;
};
} // namespace codegen
