// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#pragma once

#include "ast/FunctionStmt.h"
#include "codegen/Type.h"
#include "codegen/TypedValue.h"
#include "util/SourceLocation.h"

namespace codegen
{
/// Defined symbol
class Symbol
{
public:
    Symbol(util::SourceLocation l, std::unique_ptr<TypedValue> pValue,
           std::string pName, bool mut)
        : value(std::move(pValue)), name(std::move(pName)), isMutable(mut),
          loc(l)
    {
    }

    Symbol(const Symbol& other) = delete;
    Symbol& operator=(const Symbol& other) = delete;

    Symbol(Symbol&& other) = default;
    Symbol& operator=(Symbol&& other) = default;

    virtual ~Symbol() = default;

    virtual std::unique_ptr<Symbol> clone()
    {
        auto s = std::make_unique<Symbol>(loc, value->clone(), name, isMutable);
        s->isExport = isExport;
        return s;
    }

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
    /// Is mutable
    bool isMutable{false};
    /// Defined in
    util::SourceLocation loc;

protected:
    Symbol(util::SourceLocation l, Type* t, llvm::Value* v, std::string pName,
           TypedValue::ValueCategory cat, bool mut)
        : value(std::make_unique<TypedValue>(t, v, cat, mut)),
          name(std::move(pName)), isMutable(mut), loc(l)
    {
    }
};

class FunctionSymbol : public Symbol
{
public:
    FunctionSymbol(util::SourceLocation l, std::unique_ptr<TypedValue> pValue,
                   std::string pName, ast::FunctionPrototypeStmt* pProto)
        : Symbol(l, std::move(pValue), std::move(pName), false), proto(pProto)
    {
    }

    std::unique_ptr<Symbol> clone() override
    {
        auto s =
            std::make_unique<FunctionSymbol>(loc, value->clone(), name, proto);
        s->isExport = isExport;
        s->mangled = mangled;
        return std::move(s);
    }

    bool isFunction() const override
    {
        return true;
    }

    /// Function prototype
    ast::FunctionPrototypeStmt* proto;
    /// Is name mangled
    bool mangled{true};
};
} // namespace codegen
