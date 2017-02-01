// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#include "codegen/Type.h"
#include "codegen/TypeTable.h"
#include "codegen/TypedValue.h"

namespace codegen
{
std::unique_ptr<TypedValue> Type::implicitCast(llvm::IRBuilder<>& builder,
                                               llvm::Value* val, Type* to) const
{
    if(basicEqual(to))
    {
        return std::make_unique<TypedValue>(to, val);
    }
    return castError("Invalid implicit cast: Cannot convert from "
                     "{} to {} implicitly (kinds: {} and {})",
                     getDecoratedName(), to->getDecoratedName(), kind,
                     to->kind);
}

std::unique_ptr<TypedValue> VoidType::cast(llvm::IRBuilder<>& builder,
                                           CastType c, llvm::Value* val,
                                           Type* to) const
{
    if(c == IMPLICIT)
    {
        return implicitCast(builder, val, to);
    }

    return castError("Invalid cast: Cannot cast void");
}

std::unique_ptr<TypedValue> IntegralType::cast(llvm::IRBuilder<>& builder,
                                               CastType c, llvm::Value* val,
                                               Type* to) const
{
    if(c == IMPLICIT)
    {
        return implicitCast(builder, val, to);
    }

    auto ret = [&](llvm::Value* v) {
        return std::make_unique<TypedValue>(to, v);
    };

    switch(to->kind)
    {
    case INT:
    case INT8:
    case INT16:
    case INT32:
    case INT64:
    case BYTE:
        return ret(builder.CreateIntCast(val, to->type, true, "casttmp"));
    case BOOL:
        return ret(builder.CreateICmpNE(val, 0, "casttmp"));
    case FLOAT:
    case F32:
    case F64:
        return ret(builder.CreateSIToFP(val, to->type, "casttmp"));
    default:
        if(c == BITCAST)
        {
            auto bitcast = builder.CreateBitCast(val, to->type, "casttmp");
            if(!bitcast)
            {
                return castError(
                    "Invalid bitcast: Cannot convert from {} to {}",
                    getDecoratedName(), to->getDecoratedName());
            }
            return ret(bitcast);
        }
        return castError("Invalid cast: Cannot convert from {} to {}",
                         getDecoratedName(), to->getDecoratedName());
    }
    llvm_unreachable("Logic error in IntegralType::cast");
}

std::unique_ptr<TypedValue> BoolType::cast(llvm::IRBuilder<>& builder,
                                           CastType c, llvm::Value* val,
                                           Type* to) const
{
    if(c == IMPLICIT)
    {
        return implicitCast(builder, val, to);
    }

    auto ret = [&](llvm::Value* v) {
        return std::make_unique<TypedValue>(to, v);
    };

    switch(to->kind)
    {
    case INT:
    case INT8:
    case INT16:
    case INT32:
    case INT64:
    case BYTE:
        return ret(builder.CreateICmpNE(val, 0, "casttmp"));
    case FLOAT:
    case F32:
    case F64:
        return ret(builder.CreateFCmpONE(
            val, llvm::ConstantFP::get(typeTable->find("float")->type, 0.0),
            "casttmp"));
    default:
        if(c == BITCAST)
        {
            auto bitcast = builder.CreateBitCast(val, to->type, "casttmp");
            if(!bitcast)
            {
                return castError(
                    "Invalid bitcast: Cannot convert from {} to {}",
                    getDecoratedName(), to->getDecoratedName());
            }
            return ret(bitcast);
        }
        return castError("Invalid cast: Cannot convert from {} to {}",
                         getDecoratedName(), to->getDecoratedName());
    }
    llvm_unreachable("Logic error in BoolType::cast");
}

std::unique_ptr<TypedValue> CharacterType::cast(llvm::IRBuilder<>& builder,
                                                CastType c, llvm::Value* val,
                                                Type* to) const
{
    if(c == IMPLICIT)
    {
        return implicitCast(builder, val, to);
    }

    auto ret = [&](llvm::Value* v) {
        return std::make_unique<TypedValue>(to, v);
    };

    if(c == BITCAST)
    {
        auto bitcast = builder.CreateBitCast(val, to->type, "casttmp");
        if(!bitcast)
        {
            return castError("Invalid bitcast: Cannot convert from {} to {}",
                             getDecoratedName(), to->getDecoratedName());
        }
        return ret(bitcast);
    }
    return castError("Invalid cast: Cannot convert from {} to {}",
                     getDecoratedName(), to->getDecoratedName());
}

std::unique_ptr<TypedValue> ByteType::cast(llvm::IRBuilder<>& builder,
                                           CastType c, llvm::Value* val,
                                           Type* to) const
{
    if(c == IMPLICIT)
    {
        return implicitCast(builder, val, to);
    }

    auto ret = [&](llvm::Value* v) {
        return std::make_unique<TypedValue>(to, v);
    };

    switch(to->kind)
    {
    case INT:
    case INT8:
    case INT16:
    case INT32:
    case INT64:
        return ret(builder.CreateIntCast(val, to->type, false, "casttmp"));
    case BOOL:
        return ret(builder.CreateICmpNE(val, 0, "casttmp"));
    default:
        if(c == BITCAST)
        {
            auto bitcast = builder.CreateBitCast(val, to->type, "casttmp");
            if(!bitcast)
            {
                return castError(
                    "Invalid bitcast: Cannot convert from {} to {}",
                    getDecoratedName(), to->getDecoratedName());
            }
            return ret(bitcast);
        }
        return castError("Invalid cast: Cannot convert from {} to {}",
                         getDecoratedName(), to->getDecoratedName());
    }
}

std::unique_ptr<TypedValue> FPType::cast(llvm::IRBuilder<>& builder, CastType c,
                                         llvm::Value* val, Type* to) const
{
    if(c == IMPLICIT)
    {
        return implicitCast(builder, val, to);
    }

    auto ret = [&](llvm::Value* v) {
        return std::make_unique<TypedValue>(to, v);
    };

    switch(to->kind)
    {
    case INT:
    case INT8:
    case INT16:
    case INT32:
    case INT64:
        return ret(builder.CreateFPToSI(val, to->type, "casttmp"));
    case BYTE:
        return ret(builder.CreateFPToUI(val, to->type, "casttmp"));
    default:
        if(c == BITCAST)
        {
            auto bitcast = builder.CreateBitCast(val, to->type, "casttmp");
            if(!bitcast)
            {
                return castError(
                    "Invalid bitcast: Cannot convert from {} to {}",
                    getDecoratedName(), to->getDecoratedName());
            }
            return ret(bitcast);
        }
        return castError("Invalid cast: Cannot convert from {} to {}",
                         getDecoratedName(), to->getDecoratedName());
    }
    llvm_unreachable("Logic error in FPType::cast");
}

std::unique_ptr<TypedValue> StringType::cast(llvm::IRBuilder<>& builder,
                                             CastType c, llvm::Value* val,
                                             Type* to) const
{
    if(c == IMPLICIT)
    {
        return implicitCast(builder, val, to);
    }

    return castError("Invalid cast: Cannot cast string");
}

std::unique_ptr<TypedValue> FunctionType::cast(llvm::IRBuilder<>& builder,
                                               CastType c, llvm::Value* val,
                                               Type* to) const
{
    if(c == IMPLICIT)
    {
        return implicitCast(builder, val, to);
    }

    return castError("Invalid cast: Cannot cast function");
}
} // namespace codegen
