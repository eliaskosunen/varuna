// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#pragma once

#include "ast/AST.h"
#include "codegen/Type.h"
#include "util/Logger.h"

namespace codegen
{
class TypeOperationBase
{
public:
    explicit TypeOperationBase(Type* pType) : type(pType)
    {
    }

    TypeOperationBase(const TypeOperationBase&) = delete;
    TypeOperationBase& operator=(const TypeOperationBase&) = delete;

    TypeOperationBase(TypeOperationBase&&) noexcept = delete;
    TypeOperationBase& operator=(TypeOperationBase&&) noexcept = delete;

    virtual ~TypeOperationBase() = default;

    template <typename... Args>
    std::nullptr_t operationError(ast::Node* node, const std::string& format,
                                  Args&&... args) const
    {
        return util::logCompilerError(node->loc, format,
                                      std::forward<Args>(args)...);
    }

    template <typename... Args>
    void operationWarning(ast::Node* node, const std::string& format,
                          Args&&... args) const
    {
        util::logCompilerWarning(node->loc, format,
                                 std::forward<Args>(args)...);
    }

    template <typename... Args>
    void operationInfo(ast::Node* node, const std::string& format,
                       Args&&... args) const
    {
        util::logCompilerInfo(node->loc, format, std::forward<Args>(args)...);
    }

    /// Assignment operations
    virtual std::unique_ptr<TypedValue>
    assignmentOperation(ast::Node* node, llvm::IRBuilder<>& builder,
                        util::OperatorType op,
                        std::vector<TypedValue*> operands) const = 0;
    /// Unary operations
    virtual std::unique_ptr<TypedValue>
    unaryOperation(ast::Node* node, llvm::IRBuilder<>& builder,
                   util::OperatorType op,
                   std::vector<TypedValue*> operands) const = 0;
    /// Binary operations
    virtual std::unique_ptr<TypedValue>
    binaryOperation(ast::Node* node, llvm::IRBuilder<>& builder,
                    util::OperatorType op,
                    std::vector<TypedValue*> operands) const = 0;
    /// Arbitrary-operand operations
    virtual std::unique_ptr<TypedValue>
    arbitraryOperation(ast::Node* node, llvm::IRBuilder<>& builder,
                       util::OperatorType op,
                       std::vector<TypedValue*> operands) const = 0;

    Type* type;
};

class VoidTypeOperation : public TypeOperationBase
{
public:
    explicit VoidTypeOperation(VoidType* pType) : TypeOperationBase(pType)
    {
    }

    std::unique_ptr<TypedValue>
    assignmentOperation(ast::Node* node, llvm::IRBuilder<>& builder,
                        util::OperatorType op,
                        std::vector<TypedValue*> operands) const override;
    std::unique_ptr<TypedValue>
    unaryOperation(ast::Node* node, llvm::IRBuilder<>& builder,
                   util::OperatorType op,
                   std::vector<TypedValue*> operands) const override;
    std::unique_ptr<TypedValue>
    binaryOperation(ast::Node* node, llvm::IRBuilder<>& builder,
                    util::OperatorType op,
                    std::vector<TypedValue*> operands) const override;
    std::unique_ptr<TypedValue>
    arbitraryOperation(ast::Node* node, llvm::IRBuilder<>& builder,
                       util::OperatorType op,
                       std::vector<TypedValue*> operands) const override;
};

class IntegralTypeOperation : public TypeOperationBase
{
public:
    explicit IntegralTypeOperation(IntegralType* pType)
        : TypeOperationBase(pType)
    {
    }

    std::unique_ptr<TypedValue>
    assignmentOperation(ast::Node* node, llvm::IRBuilder<>& builder,
                        util::OperatorType op,
                        std::vector<TypedValue*> operands) const override;
    std::unique_ptr<TypedValue>
    unaryOperation(ast::Node* node, llvm::IRBuilder<>& builder,
                   util::OperatorType op,
                   std::vector<TypedValue*> operands) const override;
    std::unique_ptr<TypedValue>
    binaryOperation(ast::Node* node, llvm::IRBuilder<>& builder,
                    util::OperatorType op,
                    std::vector<TypedValue*> operands) const override;
    std::unique_ptr<TypedValue>
    arbitraryOperation(ast::Node* node, llvm::IRBuilder<>& builder,
                       util::OperatorType op,
                       std::vector<TypedValue*> operands) const override;
};

class BoolTypeOperation : public TypeOperationBase
{
public:
    explicit BoolTypeOperation(BoolType* pType) : TypeOperationBase(pType)
    {
    }

    std::unique_ptr<TypedValue>
    assignmentOperation(ast::Node* node, llvm::IRBuilder<>& builder,
                        util::OperatorType op,
                        std::vector<TypedValue*> operands) const override;
    std::unique_ptr<TypedValue>
    unaryOperation(ast::Node* node, llvm::IRBuilder<>& builder,
                   util::OperatorType op,
                   std::vector<TypedValue*> operands) const override;
    std::unique_ptr<TypedValue>
    binaryOperation(ast::Node* node, llvm::IRBuilder<>& builder,
                    util::OperatorType op,
                    std::vector<TypedValue*> operands) const override;
    std::unique_ptr<TypedValue>
    arbitraryOperation(ast::Node* node, llvm::IRBuilder<>& builder,
                       util::OperatorType op,
                       std::vector<TypedValue*> operands) const override;
};

class FPTypeOperation : public TypeOperationBase
{
public:
    explicit FPTypeOperation(FPType* pType) : TypeOperationBase(pType)
    {
    }

    std::unique_ptr<TypedValue>
    assignmentOperation(ast::Node* node, llvm::IRBuilder<>& builder,
                        util::OperatorType op,
                        std::vector<TypedValue*> operands) const override;
    std::unique_ptr<TypedValue>
    unaryOperation(ast::Node* node, llvm::IRBuilder<>& builder,
                   util::OperatorType op,
                   std::vector<TypedValue*> operands) const override;
    std::unique_ptr<TypedValue>
    binaryOperation(ast::Node* node, llvm::IRBuilder<>& builder,
                    util::OperatorType op,
                    std::vector<TypedValue*> operands) const override;
    std::unique_ptr<TypedValue>
    arbitraryOperation(ast::Node* node, llvm::IRBuilder<>& builder,
                       util::OperatorType op,
                       std::vector<TypedValue*> operands) const override;
};

class CharacterTypeOperation : public TypeOperationBase
{
public:
    explicit CharacterTypeOperation(CharacterType* pType)
        : TypeOperationBase(pType)
    {
    }

    std::unique_ptr<TypedValue>
    assignmentOperation(ast::Node* node, llvm::IRBuilder<>& builder,
                        util::OperatorType op,
                        std::vector<TypedValue*> operands) const override;
    std::unique_ptr<TypedValue>
    unaryOperation(ast::Node* node, llvm::IRBuilder<>& builder,
                   util::OperatorType op,
                   std::vector<TypedValue*> operands) const override;
    std::unique_ptr<TypedValue>
    binaryOperation(ast::Node* node, llvm::IRBuilder<>& builder,
                    util::OperatorType op,
                    std::vector<TypedValue*> operands) const override;
    std::unique_ptr<TypedValue>
    arbitraryOperation(ast::Node* node, llvm::IRBuilder<>& builder,
                       util::OperatorType op,
                       std::vector<TypedValue*> operands) const override;
};

class ByteTypeOperation : public TypeOperationBase
{
public:
    explicit ByteTypeOperation(ByteType* pType) : TypeOperationBase(pType)
    {
    }

    std::unique_ptr<TypedValue>
    assignmentOperation(ast::Node* node, llvm::IRBuilder<>& builder,
                        util::OperatorType op,
                        std::vector<TypedValue*> operands) const override;
    std::unique_ptr<TypedValue>
    unaryOperation(ast::Node* node, llvm::IRBuilder<>& builder,
                   util::OperatorType op,
                   std::vector<TypedValue*> operands) const override;
    std::unique_ptr<TypedValue>
    binaryOperation(ast::Node* node, llvm::IRBuilder<>& builder,
                    util::OperatorType op,
                    std::vector<TypedValue*> operands) const override;
    std::unique_ptr<TypedValue>
    arbitraryOperation(ast::Node* node, llvm::IRBuilder<>& builder,
                       util::OperatorType op,
                       std::vector<TypedValue*> operands) const override;
};

class StringTypeOperation : public TypeOperationBase
{
public:
    explicit StringTypeOperation(StringType* pType) : TypeOperationBase(pType)
    {
    }

    std::unique_ptr<TypedValue>
    assignmentOperation(ast::Node* node, llvm::IRBuilder<>& builder,
                        util::OperatorType op,
                        std::vector<TypedValue*> operands) const override;
    std::unique_ptr<TypedValue>
    unaryOperation(ast::Node* node, llvm::IRBuilder<>& builder,
                   util::OperatorType op,
                   std::vector<TypedValue*> operands) const override;
    std::unique_ptr<TypedValue>
    binaryOperation(ast::Node* node, llvm::IRBuilder<>& builder,
                    util::OperatorType op,
                    std::vector<TypedValue*> operands) const override;
    std::unique_ptr<TypedValue>
    arbitraryOperation(ast::Node* node, llvm::IRBuilder<>& builder,
                       util::OperatorType op,
                       std::vector<TypedValue*> operands) const override;
};

class CStringTypeOperation : public TypeOperationBase
{
public:
    explicit CStringTypeOperation(CStringType* pType) : TypeOperationBase(pType)
    {
    }

    std::unique_ptr<TypedValue>
    assignmentOperation(ast::Node* node, llvm::IRBuilder<>& builder,
                        util::OperatorType op,
                        std::vector<TypedValue*> operands) const override;
    std::unique_ptr<TypedValue>
    unaryOperation(ast::Node* node, llvm::IRBuilder<>& builder,
                   util::OperatorType op,
                   std::vector<TypedValue*> operands) const override;
    std::unique_ptr<TypedValue>
    binaryOperation(ast::Node* node, llvm::IRBuilder<>& builder,
                    util::OperatorType op,
                    std::vector<TypedValue*> operands) const override;
    std::unique_ptr<TypedValue>
    arbitraryOperation(ast::Node* node, llvm::IRBuilder<>& builder,
                       util::OperatorType op,
                       std::vector<TypedValue*> operands) const override;
};

class FunctionTypeOperation : public TypeOperationBase
{
public:
    explicit FunctionTypeOperation(FunctionType* pType)
        : TypeOperationBase(pType)
    {
    }

    std::unique_ptr<TypedValue>
    assignmentOperation(ast::Node* node, llvm::IRBuilder<>& builder,
                        util::OperatorType op,
                        std::vector<TypedValue*> operands) const override;
    std::unique_ptr<TypedValue>
    unaryOperation(ast::Node* node, llvm::IRBuilder<>& builder,
                   util::OperatorType op,
                   std::vector<TypedValue*> operands) const override;
    std::unique_ptr<TypedValue>
    binaryOperation(ast::Node* node, llvm::IRBuilder<>& builder,
                    util::OperatorType op,
                    std::vector<TypedValue*> operands) const override;
    std::unique_ptr<TypedValue>
    arbitraryOperation(ast::Node* node, llvm::IRBuilder<>& builder,
                       util::OperatorType op,
                       std::vector<TypedValue*> operands) const override;
};
} // namespace codegen
