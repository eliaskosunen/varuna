// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#pragma once

#include "ast/ASTFunctionStatement.h"
#include "codegen/Type.h"
#include "codegen/TypedValue.h"

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

    Symbol(const Symbol& other)
        : value(other.value->clone()), // value is cloned
          name(other.name), isExport(other.isExport)
    {
    }
    Symbol& operator=(const Symbol& other)
    {
        Symbol(other).swap(*this);
        return *this;
    }

    Symbol(Symbol&& other) noexcept
        : value(std::move(other.value)), name(std::move(other.name)),
          isExport(std::move(other.isExport))
    {
        other.value = nullptr;
        other.name = "";
    }
    Symbol& operator=(Symbol&& other) noexcept
    {
        Symbol(std::move(other)).swap(*this);
        return *this;
    }

    virtual ~Symbol() = default;

    void swap(Symbol& other) noexcept
    {
        std::swap(value, other.value);
        std::swap(name, other.name);
        std::swap(isExport, other.isExport);
    }

    /// Get the type of the symbol
    Type* getType() const
    {
        assert(value && "No value given for Symbol");
        return value->type;
    }

    virtual bool isFunction() const
    {
        return false;
    }

    std::unique_ptr<TypedValue> value;
    std::string name;
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

    FunctionSymbol(const FunctionSymbol& other)
        : Symbol(other), proto(other.proto)
    {
    }
    FunctionSymbol& operator=(const FunctionSymbol& other)
    {
        FunctionSymbol(other).swap(*this);
        return *this;
    }

    FunctionSymbol(FunctionSymbol&& other) noexcept
        : Symbol(std::move(other)), proto(std::move(other.proto))
    {
        other.value = nullptr;
        other.name = "";
        other.proto = nullptr;
    }
    FunctionSymbol& operator=(FunctionSymbol&& other) noexcept
    {
        FunctionSymbol(std::move(other)).swap(*this);
        return *this;
    }

    void swap(FunctionSymbol& other) noexcept
    {
        std::swap(value, other.value);
        std::swap(name, other.name);
        std::swap(isExport, other.isExport);
        std::swap(proto, other.proto);
    }

    bool isFunction() const override
    {
        return true;
    }

    ast::ASTFunctionPrototypeStatement* proto;
};
} // namespace codegen
