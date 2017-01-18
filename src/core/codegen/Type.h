// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#pragma once

#include "core/ast/ASTFunctionStatement.h"
#include "core/lexer/Token.h"
#include "util/Logger.h"
#include <llvm/IR/DIBuilder.h>
#include <llvm/IR/IRBuilder.h>
#include <tuple>

namespace core
{
namespace codegen
{
    struct TypedValue;
    class TypeOperationBase;
    class Type;
    class TypeTable;

    class Type
    {
    public:
        enum Kind
        {
            VOID,
            INT,
            INT8,
            INT16,
            INT32,
            INT64,
            BOOL,
            FLOAT,
            F32,
            F64,
            BYTE,
            CHAR,
            STRING,
            FUNCTION
        };
        enum CastType
        {
            IMPLICIT,
            CAST,
            BITCAST
        };

        /*
        tuple<bool, bool, CastOps>
        isError
        castNeeded
        castOperation
         */
        // using CastTuple = std::tuple<bool, bool, llvm::Instruction::CastOps>;
        using CastTuple = std::tuple<bool, std::unique_ptr<TypedValue>>;
        const decltype(llvm::Instruction::BitCast) defaultCast =
            llvm::Instruction::BitCast;

        Type(TypeTable* list, std::unique_ptr<TypeOperationBase> op, Kind k,
             llvm::LLVMContext& c, llvm::Type* t, llvm::DIType* d,
             std::string n, bool mut = false);
        Type(const Type& t) = default;
        Type& operator=(const Type& t) = default;
        Type(Type&&) noexcept = default;
        Type& operator=(Type&&) noexcept = default;
        virtual ~Type() noexcept;

        template <typename... Args>
        std::nullptr_t castError(const std::string& format,
                                 Args&&... args) const
        {
            util::logger->error(format.c_str(), std::forward<Args>(args)...);
            return nullptr;
        }

        virtual std::unique_ptr<TypedValue> cast(llvm::IRBuilder<>& builder,
                                                 CastType c, llvm::Value* val,
                                                 Type* to) const = 0;
        // virtual CastTuple cast(CastType c, Type* to) const = 0;

        bool isSameOrImplicitlyCastable(llvm::IRBuilder<>& builder,
                                        llvm::Value* val, Type* to) const;

        virtual bool isSized() const
        {
            return true;
        }
        virtual bool isIntegral() const = 0;
        virtual bool isFloatingPoint() const = 0;

        bool equal(const Type& t) const
        {
            return kind == t.kind && getDecoratedName() == t.getDecoratedName();
        }
        bool inequal(const Type& t) const
        {
            return equal(t);
        }
        bool equal(Type* t) const
        {
            return kind == t->kind &&
                   getDecoratedName() == t->getDecoratedName();
        }
        bool inequal(Type* t) const
        {
            return equal(t);
        }

        bool operator==(const Type& t) const
        {
            return equal(t);
        }
        bool operator!=(const Type& t) const
        {
            return inequal(t);
        }

        bool basicEqual(const Type& t) const
        {
            return kind == t.kind && getName() == t.getName();
        }
        bool basicInequal(const Type& t) const
        {
            return !basicEqual(t);
        }
        bool basicEqual(Type* t) const
        {
            return kind == t->kind && getName() == t->getName();
        }
        bool basicInequal(Type* t) const
        {
            return !basicEqual(t);
        }

        TypeTable* typeTable;
        llvm::LLVMContext& context;
        llvm::Type* type;
        llvm::DIType* dtype;
        Kind kind;
        bool isMutable{false};

        const std::string& getName() const
        {
            return name;
        }
        const std::string& getDecoratedName() const
        {
            if(isMutable)
            {
                static std::string n = fmt::format("{} mut", name);
                return n;
            }
            return name;
        }

        TypeOperationBase* getOperations() const;

    protected:
        std::unique_ptr<TypedValue> implicitCast(llvm::IRBuilder<>& builder,
                                                 llvm::Value* val,
                                                 Type* to) const;

    private:
        std::string name;

        std::unique_ptr<TypeOperationBase> operations;
    };

    class VoidType : public Type
    {
    public:
        VoidType(TypeTable* list, llvm::LLVMContext& c, llvm::DIBuilder&,
                 bool mut = false);

        std::unique_ptr<TypedValue> cast(llvm::IRBuilder<>& builder, CastType c,
                                         llvm::Value* val,
                                         Type* to) const override;

        bool isSized() const override;
        bool isIntegral() const override;
        bool isFloatingPoint() const override;
    };

    class IntegralType : public Type
    {
    public:
        IntegralType(TypeTable* list, uint32_t w, Kind k, llvm::LLVMContext& c,
                     llvm::Type* t, llvm::DIType* d, const std::string& n,
                     bool mut = false);

        std::unique_ptr<TypedValue> cast(llvm::IRBuilder<>& builder, CastType c,
                                         llvm::Value* val,
                                         Type* to) const override;

        bool isIntegral() const override;
        bool isFloatingPoint() const override;

        uint32_t width;
    };

    class IntType : public IntegralType
    {
    public:
        IntType(TypeTable* list, llvm::LLVMContext& c,
                llvm::DIBuilder& dbuilder, bool mut = false);
    };

    class Int8Type : public IntegralType
    {
    public:
        Int8Type(TypeTable* list, llvm::LLVMContext& c,
                 llvm::DIBuilder& dbuilder, bool mut = false);
    };

    class Int16Type : public IntegralType
    {
    public:
        Int16Type(TypeTable* list, llvm::LLVMContext& c,
                  llvm::DIBuilder& dbuilder, bool mut = false);
    };

    class Int32Type : public IntegralType
    {
    public:
        Int32Type(TypeTable* list, llvm::LLVMContext& c,
                  llvm::DIBuilder& dbuilder, bool mut = false);
    };

    class Int64Type : public IntegralType
    {
    public:
        Int64Type(TypeTable* list, llvm::LLVMContext& c,
                  llvm::DIBuilder& dbuilder, bool mut = false);
    };

    class BoolType : public Type
    {
    public:
        BoolType(TypeTable* list, llvm::LLVMContext& c,
                 llvm::DIBuilder& dbuilder, bool mut = false);

        std::unique_ptr<TypedValue> cast(llvm::IRBuilder<>& builder, CastType c,
                                         llvm::Value* val,
                                         Type* to) const override;

        bool isIntegral() const override;
        bool isFloatingPoint() const override;
    };

    class CharType : public Type
    {
    public:
        CharType(TypeTable* list, llvm::LLVMContext& c,
                 llvm::DIBuilder& dbuilder, bool mut = false);

        std::unique_ptr<TypedValue> cast(llvm::IRBuilder<>& builder, CastType c,
                                         llvm::Value* val,
                                         Type* to) const override;

        bool isIntegral() const override;
        bool isFloatingPoint() const override;
    };

    class ByteType : public Type
    {
    public:
        ByteType(TypeTable* list, llvm::LLVMContext& c,
                 llvm::DIBuilder& dbuilder, bool mut = false);

        std::unique_ptr<TypedValue> cast(llvm::IRBuilder<>& builder, CastType c,
                                         llvm::Value* val,
                                         Type* to) const override;

        bool isIntegral() const override;
        bool isFloatingPoint() const override;
    };

    class FPType : public Type
    {
    public:
        FPType(TypeTable* list, uint32_t w, Kind k, llvm::LLVMContext& c,
               llvm::Type* t, llvm::DIType* d, const std::string& n,
               bool mut = false);

        std::unique_ptr<TypedValue> cast(llvm::IRBuilder<>& builder, CastType c,
                                         llvm::Value* val,
                                         Type* to) const override;

        bool isIntegral() const override;
        bool isFloatingPoint() const override;

        uint32_t width;
    };

    class FloatType : public FPType
    {
    public:
        FloatType(TypeTable* list, llvm::LLVMContext& c,
                  llvm::DIBuilder& dbuilder, bool mut = false);
    };

    class F32Type : public FPType
    {
    public:
        F32Type(TypeTable* list, llvm::LLVMContext& c,
                llvm::DIBuilder& dbuilder, bool mut = false);
    };

    class F64Type : public FPType
    {
    public:
        F64Type(TypeTable* list, llvm::LLVMContext& c,
                llvm::DIBuilder& dbuilder, bool mut = false);
    };

    class StringType : public Type
    {
    public:
        StringType(TypeTable* list, llvm::LLVMContext& c, llvm::DIBuilder&,
                   bool mut = false);

        std::unique_ptr<TypedValue> cast(llvm::IRBuilder<>& builder, CastType c,
                                         llvm::Value* val,
                                         Type* to) const override;

        bool isIntegral() const override;
        bool isFloatingPoint() const override;
    };

    class FunctionType : public Type
    {
    public:
        FunctionType(TypeTable* list, llvm::LLVMContext& c,
                     llvm::DIBuilder& dbuilder, Type* pReturnType,
                     std::vector<Type*> pParams);

        static llvm::FunctionType*
        getLLVMFunctionType(Type* returnType, const std::vector<Type*>& params);
        static std::string paramVectorToString(const std::vector<Type*>& params,
                                               bool addBeginningComma = false);
        static std::string
        functionTypeToString(Type* returnType,
                             const std::vector<Type*>& params);

        std::unique_ptr<TypedValue> cast(llvm::IRBuilder<>& builder, CastType c,
                                         llvm::Value* val,
                                         Type* to) const override;

        bool isIntegral() const override;
        bool isFloatingPoint() const override;

        Type* returnType;
        std::vector<Type*> params;
    };
} // namespace codegen
} // namespace core
