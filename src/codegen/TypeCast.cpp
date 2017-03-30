// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#include "ast/OperatorExpr.h"
#include "codegen/Type.h"
#include "codegen/TypeTable.h"
#include "codegen/TypedValue.h"

namespace codegen
{
std::unique_ptr<TypedValue> Type::implicitCast(ast::Node* node,
                                               llvm::IRBuilder<>& /*builder*/,
                                               TypedValue* val, Type* to) const
{
    if(basicEqual(to))
    {
        return val->clone();
    }
    return castError(node, "Invalid implicit cast: Cannot convert from "
                           "{} to {} implicitly (kinds: {} and {})",
                     getDecoratedName(), to->getDecoratedName(), kind,
                     to->kind.get());
}

std::unique_ptr<TypedValue> VoidType::cast(ast::Node* node,
                                           llvm::IRBuilder<>& builder,
                                           CastType c, TypedValue* val,
                                           Type* to) const
{
    if(c == IMPLICIT)
    {
        return implicitCast(node, builder, val, to);
    }

    return castError(node, "Invalid cast: Cannot cast void");
}

std::unique_ptr<TypedValue> IntegralType::cast(ast::Node* node,
                                               llvm::IRBuilder<>& builder,
                                               CastType c, TypedValue* val,
                                               Type* to) const
{
    if(c == IMPLICIT)
    {
        return implicitCast(node, builder, val, to);
    }

    auto ret = [&](llvm::Value* v) {
        return std::make_unique<TypedValue>(to, v, TypedValue::RVALUE,
                                            val->isMutable);
    };

    switch(to->kind.get())
    {
    case INT8:
    case INT16:
    case INT32:
    case INT64:
    case BYTE:
        return ret(
            builder.CreateIntCast(val->value, to->type, true, "casttmp"));
    case BOOL:
        return ret(builder.CreateICmpNE(val->value, nullptr, "casttmp"));
    case F32:
    case F64:
        return ret(builder.CreateSIToFP(val->value, to->type, "casttmp"));
    default:
        if(c == BITCAST)
        {
            auto bitcast =
                builder.CreateBitCast(val->value, to->type, "casttmp");
            if(!bitcast)
            {
                return castError(
                    node, "Invalid bitcast: Cannot convert from {} to {}",
                    getDecoratedName(), to->getDecoratedName());
            }
            return ret(bitcast);
        }
        return castError(node, "Invalid cast: Cannot convert from {} to {}",
                         getDecoratedName(), to->getDecoratedName());
    }
}

std::unique_ptr<TypedValue> BoolType::cast(ast::Node* node,
                                           llvm::IRBuilder<>& builder,
                                           CastType c, TypedValue* val,
                                           Type* to) const
{
    if(c == IMPLICIT)
    {
        return implicitCast(node, builder, val, to);
    }

    auto ret = [&](llvm::Value* v) {
        return std::make_unique<TypedValue>(to, v, TypedValue::RVALUE,
                                            val->isMutable);
    };

    switch(to->kind.get())
    {
    case INT8:
    case INT16:
    case INT32:
    case INT64:
    case BYTE:
        return ret(builder.CreateICmpNE(val->value, nullptr, "casttmp"));
    case F32:
    case F64:
        return ret(builder.CreateFCmpONE(
            val->value,
            llvm::ConstantFP::get(typeTable->find("float")->type, 0.0),
            "casttmp"));
    default:
        if(c == BITCAST)
        {
            auto bitcast =
                builder.CreateBitCast(val->value, to->type, "casttmp");
            if(!bitcast)
            {
                return castError(
                    node, "Invalid bitcast: Cannot convert from {} to {}",
                    getDecoratedName(), to->getDecoratedName());
            }
            return ret(bitcast);
        }
        return castError(node, "Invalid cast: Cannot convert from {} to {}",
                         getDecoratedName(), to->getDecoratedName());
    }
}

std::unique_ptr<TypedValue> CharacterType::cast(ast::Node* node,
                                                llvm::IRBuilder<>& builder,
                                                CastType c, TypedValue* val,
                                                Type* to) const
{
    if(c == IMPLICIT)
    {
        return implicitCast(node, builder, val, to);
    }

    auto ret = [&](llvm::Value* v) {
        return std::make_unique<TypedValue>(to, v, TypedValue::RVALUE,
                                            val->isMutable);
    };

    if(c == BITCAST)
    {
        auto bitcast = builder.CreateBitCast(val->value, to->type, "casttmp");
        if(!bitcast)
        {
            return castError(node,
                             "Invalid bitcast: Cannot convert from {} to {}",
                             getDecoratedName(), to->getDecoratedName());
        }
        return ret(bitcast);
    }
    return castError(node, "Invalid cast: Cannot convert from {} to {}",
                     getDecoratedName(), to->getDecoratedName());
}

std::unique_ptr<TypedValue> ByteType::cast(ast::Node* node,
                                           llvm::IRBuilder<>& builder,
                                           CastType c, TypedValue* val,
                                           Type* to) const
{
    if(c == IMPLICIT)
    {
        return implicitCast(node, builder, val, to);
    }

    auto ret = [&](llvm::Value* v) {
        return std::make_unique<TypedValue>(to, v, TypedValue::RVALUE,
                                            val->isMutable);
    };

    switch(to->kind.get())
    {
    case INT8:
    case INT16:
    case INT32:
    case INT64:
        return ret(
            builder.CreateIntCast(val->value, to->type, false, "casttmp"));
    case BOOL:
        return ret(builder.CreateICmpNE(val->value, nullptr, "casttmp"));
    default:
        if(c == BITCAST)
        {
            auto bitcast =
                builder.CreateBitCast(val->value, to->type, "casttmp");
            if(!bitcast)
            {
                return castError(
                    node, "Invalid bitcast: Cannot convert from {} to {}",
                    getDecoratedName(), to->getDecoratedName());
            }
            return ret(bitcast);
        }
        return castError(node, "Invalid cast: Cannot convert from {} to {}",
                         getDecoratedName(), to->getDecoratedName());
    }
}

std::unique_ptr<TypedValue> FPType::cast(ast::Node* node,
                                         llvm::IRBuilder<>& builder, CastType c,
                                         TypedValue* val, Type* to) const
{
    if(c == IMPLICIT)
    {
        return implicitCast(node, builder, val, to);
    }

    auto ret = [&](llvm::Value* v) {
        return std::make_unique<TypedValue>(to, v, TypedValue::RVALUE,
                                            val->isMutable);
    };

    switch(to->kind.get())
    {
    case INT8:
    case INT16:
    case INT32:
    case INT64:
        return ret(builder.CreateFPToSI(val->value, to->type, "casttmp"));
    case BYTE:
        return ret(builder.CreateFPToUI(val->value, to->type, "casttmp"));
    default:
        if(c == BITCAST)
        {
            auto bitcast =
                builder.CreateBitCast(val->value, to->type, "casttmp");
            if(!bitcast)
            {
                return castError(
                    node, "Invalid bitcast: Cannot convert from {} to {}",
                    getDecoratedName(), to->getDecoratedName());
            }
            return ret(bitcast);
        }
        return castError(node, "Invalid cast: Cannot convert from {} to {}",
                         getDecoratedName(), to->getDecoratedName());
    }
}

std::unique_ptr<TypedValue> StringType::cast(ast::Node* node,
                                             llvm::IRBuilder<>& builder,
                                             CastType c, TypedValue* val,
                                             Type* to) const
{
    if(c == IMPLICIT)
    {
        return implicitCast(node, builder, val, to);
    }

    return castError(node, "Invalid cast: Cannot cast string");
}

std::unique_ptr<TypedValue> CStringType::cast(ast::Node* node,
                                              llvm::IRBuilder<>& builder,
                                              CastType c, TypedValue* val,
                                              Type* to) const
{
    if(c == IMPLICIT)
    {
        return implicitCast(node, builder, val, to);
    }

    return castError(node, "Invalid cast: Cannot cast string");
}

std::unique_ptr<TypedValue> FunctionType::cast(ast::Node* node,
                                               llvm::IRBuilder<>& builder,
                                               CastType c, TypedValue* val,
                                               Type* to) const
{
    if(c == IMPLICIT)
    {
        return implicitCast(node, builder, val, to);
    }

    return castError(node, "Invalid cast: Cannot cast function");
}

std::unique_ptr<TypedValue> AliasType::cast(ast::Node* node,
                                            llvm::IRBuilder<>& builder,
                                            CastType c, TypedValue* val,
                                            Type* to) const
{
    return underlying->cast(node, builder, c, val, to);
}
} // namespace codegen
