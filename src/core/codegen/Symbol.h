// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#pragma once

#include "core/ast/ASTFunctionStatement.h"
#include "core/codegen/Type.h"
#include "core/codegen/TypedValue.h"

namespace core
{
namespace codegen
{
    class Symbol
    {
    public:
        Symbol(std::unique_ptr<TypedValue> pValue, std::string pName)
            : value(std::move(pValue)), name(std::move(pName))
        {
        }
        Symbol(Type* t, llvm::Value* v, std::string pName)
            : value(std::make_unique<TypedValue>(t, v)), name(std::move(pName))
        {
        }

        Symbol(const Symbol&) = delete;
        Symbol& operator=(const Symbol&) = delete;

        Symbol(Symbol&&) = default;
        Symbol& operator=(Symbol&&) = default;

        virtual ~Symbol() = default;

        Type* getType() const
        {
            assert(value && "No value given for Symbol");
            return value->type;
        }

        std::unique_ptr<TypedValue> value;
        const std::string name;
    };

    class FunctionSymbol : public Symbol
    {
    public:
        FunctionSymbol(std::unique_ptr<TypedValue> pValue, std::string pName,
                       ast::ASTFunctionPrototypeStatement* pProto)
            : Symbol(std::move(pValue), std::move(pName)), proto(pProto)
        {
        }
        FunctionSymbol(Type* t, llvm::Value* v, std::string pName,
                       ast::ASTFunctionPrototypeStatement* pProto)
            : Symbol(t, v, std::move(pName)), proto(pProto)
        {
        }

        ast::ASTFunctionPrototypeStatement* proto;
    };
} // namespace codegen
} // namespace core
