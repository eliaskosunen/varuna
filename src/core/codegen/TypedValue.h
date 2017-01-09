// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#pragma once

#include "core/codegen/Type.h"
#include <llvm/IR/Value.h>

namespace core
{
namespace codegen
{
    struct TypedValue
    {
        TypedValue(Type* t, llvm::Value* v) : type(t), value(v)
        {
        }

        Type* type;
        llvm::Value* value;
    };
} // namespace codegen
} // namespace core
