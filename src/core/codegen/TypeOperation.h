// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#pragma once

#include "core/codegen/Type.h"

namespace core
{
namespace codegen
{
    class TypeOperationBase
    {
    public:
        TypeOperationBase(Type* pType) : type(pType)
        {
        }

        TypeOperationBase(const TypeOperationBase&) = delete;
        TypeOperationBase& operator=(const TypeOperationBase&) = delete;

        TypeOperationBase(TypeOperationBase&&) noexcept = default;
        TypeOperationBase& operator=(TypeOperationBase&&) noexcept = default;

        virtual ~TypeOperationBase() = default;

        template <typename... Args>
        std::nullptr_t operationError(const std::string& format,
                                      Args&&... args) const
        {
            util::logger->error(format.c_str(), std::forward<Args>(args)...);
            return nullptr;
        }

        /// Assignment operations
        virtual std::unique_ptr<TypedValue>
        assignmentOperation(llvm::IRBuilder<>& builder,
                            core::lexer::TokenType op,
                            std::vector<TypedValue*> operands) const = 0;
        /// Unary operations
        virtual std::unique_ptr<TypedValue>
        unaryOperation(llvm::IRBuilder<>& builder, core::lexer::TokenType op,
                       std::vector<TypedValue*> operands) const = 0;
        /// Binary operations
        virtual std::unique_ptr<TypedValue>
        binaryOperation(llvm::IRBuilder<>& builder, core::lexer::TokenType op,
                        std::vector<TypedValue*> operands) const = 0;
        /// Arbitrary-operand operations
        virtual std::unique_ptr<TypedValue>
        arbitraryOperation(llvm::IRBuilder<>& builder,
                           core::lexer::TokenType op,
                           std::vector<TypedValue*> operands) const = 0;

        Type* type;
    };

    class VoidTypeOperation : public TypeOperationBase
    {
    public:
        VoidTypeOperation(VoidType* pType) : TypeOperationBase(pType)
        {
        }

        std::unique_ptr<TypedValue>
        assignmentOperation(llvm::IRBuilder<>& builder,
                            core::lexer::TokenType op,
                            std::vector<TypedValue*> operands) const override;
        std::unique_ptr<TypedValue>
        unaryOperation(llvm::IRBuilder<>& builder, core::lexer::TokenType op,
                       std::vector<TypedValue*> operands) const override;
        std::unique_ptr<TypedValue>
        binaryOperation(llvm::IRBuilder<>& builder, core::lexer::TokenType op,
                        std::vector<TypedValue*> operands) const override;
        std::unique_ptr<TypedValue>
        arbitraryOperation(llvm::IRBuilder<>& builder,
                           core::lexer::TokenType op,
                           std::vector<TypedValue*> operands) const override;
    };

    class IntegralTypeOperation : public TypeOperationBase
    {
    public:
        IntegralTypeOperation(IntegralType* pType) : TypeOperationBase(pType)
        {
        }

        std::unique_ptr<TypedValue>
        assignmentOperation(llvm::IRBuilder<>& builder,
                            core::lexer::TokenType op,
                            std::vector<TypedValue*> operands) const override;
        std::unique_ptr<TypedValue>
        unaryOperation(llvm::IRBuilder<>& builder, core::lexer::TokenType op,
                       std::vector<TypedValue*> operands) const override;
        std::unique_ptr<TypedValue>
        binaryOperation(llvm::IRBuilder<>& builder, core::lexer::TokenType op,
                        std::vector<TypedValue*> operands) const override;
        std::unique_ptr<TypedValue>
        arbitraryOperation(llvm::IRBuilder<>& builder,
                           core::lexer::TokenType op,
                           std::vector<TypedValue*> operands) const override;
    };

    class BoolTypeOperation : public TypeOperationBase
    {
    public:
        BoolTypeOperation(BoolType* pType) : TypeOperationBase(pType)
        {
        }

        std::unique_ptr<TypedValue>
        assignmentOperation(llvm::IRBuilder<>& builder,
                            core::lexer::TokenType op,
                            std::vector<TypedValue*> operands) const override;
        std::unique_ptr<TypedValue>
        unaryOperation(llvm::IRBuilder<>& builder, core::lexer::TokenType op,
                       std::vector<TypedValue*> operands) const override;
        std::unique_ptr<TypedValue>
        binaryOperation(llvm::IRBuilder<>& builder, core::lexer::TokenType op,
                        std::vector<TypedValue*> operands) const override;
        std::unique_ptr<TypedValue>
        arbitraryOperation(llvm::IRBuilder<>& builder,
                           core::lexer::TokenType op,
                           std::vector<TypedValue*> operands) const override;
    };

    class FPTypeOperation : public TypeOperationBase
    {
    public:
        FPTypeOperation(FPType* pType) : TypeOperationBase(pType)
        {
        }

        std::unique_ptr<TypedValue>
        assignmentOperation(llvm::IRBuilder<>& builder,
                            core::lexer::TokenType op,
                            std::vector<TypedValue*> operands) const override;
        std::unique_ptr<TypedValue>
        unaryOperation(llvm::IRBuilder<>& builder, core::lexer::TokenType op,
                       std::vector<TypedValue*> operands) const override;
        std::unique_ptr<TypedValue>
        binaryOperation(llvm::IRBuilder<>& builder, core::lexer::TokenType op,
                        std::vector<TypedValue*> operands) const override;
        std::unique_ptr<TypedValue>
        arbitraryOperation(llvm::IRBuilder<>& builder,
                           core::lexer::TokenType op,
                           std::vector<TypedValue*> operands) const override;
    };

    class CharTypeOperation : public TypeOperationBase
    {
    public:
        CharTypeOperation(CharType* pType) : TypeOperationBase(pType)
        {
        }

        std::unique_ptr<TypedValue>
        assignmentOperation(llvm::IRBuilder<>& builder,
                            core::lexer::TokenType op,
                            std::vector<TypedValue*> operands) const override;
        std::unique_ptr<TypedValue>
        unaryOperation(llvm::IRBuilder<>& builder, core::lexer::TokenType op,
                       std::vector<TypedValue*> operands) const override;
        std::unique_ptr<TypedValue>
        binaryOperation(llvm::IRBuilder<>& builder, core::lexer::TokenType op,
                        std::vector<TypedValue*> operands) const override;
        std::unique_ptr<TypedValue>
        arbitraryOperation(llvm::IRBuilder<>& builder,
                           core::lexer::TokenType op,
                           std::vector<TypedValue*> operands) const override;
    };

    class ByteTypeOperation : public TypeOperationBase
    {
    public:
        ByteTypeOperation(ByteType* pType) : TypeOperationBase(pType)
        {
        }

        std::unique_ptr<TypedValue>
        assignmentOperation(llvm::IRBuilder<>& builder,
                            core::lexer::TokenType op,
                            std::vector<TypedValue*> operands) const override;
        std::unique_ptr<TypedValue>
        unaryOperation(llvm::IRBuilder<>& builder, core::lexer::TokenType op,
                       std::vector<TypedValue*> operands) const override;
        std::unique_ptr<TypedValue>
        binaryOperation(llvm::IRBuilder<>& builder, core::lexer::TokenType op,
                        std::vector<TypedValue*> operands) const override;
        std::unique_ptr<TypedValue>
        arbitraryOperation(llvm::IRBuilder<>& builder,
                           core::lexer::TokenType op,
                           std::vector<TypedValue*> operands) const override;
    };

    class StringTypeOperation : public TypeOperationBase
    {
    public:
        StringTypeOperation(StringType* pType) : TypeOperationBase(pType)
        {
        }

        std::unique_ptr<TypedValue>
        assignmentOperation(llvm::IRBuilder<>& builder,
                            core::lexer::TokenType op,
                            std::vector<TypedValue*> operands) const override;
        std::unique_ptr<TypedValue>
        unaryOperation(llvm::IRBuilder<>& builder, core::lexer::TokenType op,
                       std::vector<TypedValue*> operands) const override;
        std::unique_ptr<TypedValue>
        binaryOperation(llvm::IRBuilder<>& builder, core::lexer::TokenType op,
                        std::vector<TypedValue*> operands) const override;
        std::unique_ptr<TypedValue>
        arbitraryOperation(llvm::IRBuilder<>& builder,
                           core::lexer::TokenType op,
                           std::vector<TypedValue*> operands) const override;
    };

    class FunctionTypeOperation : public TypeOperationBase
    {
    public:
        FunctionTypeOperation(FunctionType* pType) : TypeOperationBase(pType)
        {
        }

        std::unique_ptr<TypedValue>
        assignmentOperation(llvm::IRBuilder<>& builder,
                            core::lexer::TokenType op,
                            std::vector<TypedValue*> operands) const override;
        std::unique_ptr<TypedValue>
        unaryOperation(llvm::IRBuilder<>& builder, core::lexer::TokenType op,
                       std::vector<TypedValue*> operands) const override;
        std::unique_ptr<TypedValue>
        binaryOperation(llvm::IRBuilder<>& builder, core::lexer::TokenType op,
                        std::vector<TypedValue*> operands) const override;
        std::unique_ptr<TypedValue>
        arbitraryOperation(llvm::IRBuilder<>& builder,
                           core::lexer::TokenType op,
                           std::vector<TypedValue*> operands) const override;
    };
} // namespace codegen
} // namespace core
