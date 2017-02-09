// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#pragma once

#include "ast/ASTFunctionStatement.h"
#include "codegen/Type.h"
#include "codegen/TypedValue.h"

namespace codegen
{
/// Defined symbol
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

    Symbol(const Symbol& other) = delete;
    Symbol& operator=(const Symbol& other) = delete;

    Symbol(Symbol&& other) = default;
    Symbol& operator=(Symbol&& other) = default;

    virtual ~Symbol() = default;

    /**
     * Get the type of the symbol
     * @return Type
     */
    Type* getType() const
    {
        assert(value && "No value given for Symbol");
        return value->type;
    }

    /**
     * Is symbol a function
     * @return isFunction
     */
    virtual bool isFunction() const
    {
        return false;
    }

    /// Value
    std::unique_ptr<TypedValue> value;
    /// Name
    std::string name;
    /// Is exported
    bool isExport{false};
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

    FunctionSymbol(const FunctionSymbol& other) = delete;
    FunctionSymbol& operator=(const FunctionSymbol& other) = delete;

    FunctionSymbol(FunctionSymbol&& other) = default;
    FunctionSymbol& operator=(FunctionSymbol&& other) = default;

    ~FunctionSymbol() = default;

    bool isFunction() const override
    {
        return true;
    }

    /// Function prototype
    ast::ASTFunctionPrototypeStatement* proto;
};
} // namespace codegen
