// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#include "core/codegen/TypeOperation.h"
#include "core/codegen/Symbol.h"
#include "core/codegen/Type.h"
#include "core/codegen/TypeTable.h"
#include "core/codegen/TypedValue.h"

namespace core
{
namespace codegen
{
    std::unique_ptr<TypedValue> VoidTypeOperation::assignmentOperation(
        llvm::IRBuilder<>& builder, core::lexer::TokenType op,
        std::vector<std::unique_ptr<TypedValue>> operands) const
    {
        return operationError(
            "Invalid operation: Cannot make any operations on void");
    }
    std::unique_ptr<TypedValue> VoidTypeOperation::unaryOperation(
        llvm::IRBuilder<>& builder, core::lexer::TokenType op,
        std::vector<std::unique_ptr<TypedValue>> operands) const
    {
        return operationError(
            "Invalid operation: Cannot make any operations on void");
    }
    std::unique_ptr<TypedValue> VoidTypeOperation::binaryOperation(
        llvm::IRBuilder<>& builder, core::lexer::TokenType op,
        std::vector<std::unique_ptr<TypedValue>> operands) const
    {
        return operationError(
            "Invalid operation: Cannot make any operations on void");
    }
    std::unique_ptr<TypedValue> VoidTypeOperation::arbitraryOperation(
        llvm::IRBuilder<>& builder, core::lexer::TokenType op,
        std::vector<std::unique_ptr<TypedValue>> operands) const
    {
        return operationError(
            "Invalid operation: Cannot make any operations on void");
    }

    std::unique_ptr<TypedValue> IntegralTypeOperation::assignmentOperation(
        llvm::IRBuilder<>& builder, core::lexer::TokenType op,
        std::vector<std::unique_ptr<TypedValue>> operands) const
    {
        assert(operands.size() == 2);

        if(!operands[0]->type->isMutable)
        {
            return operationError("Cannot assign to mutable lhs");
        }

        std::unique_ptr<TypedValue> rhs = nullptr;
        auto& lhs = operands[0];
        if(op != lexer::TOKEN_OPERATORA_SIMPLE)
        {
            switch(op.get())
            {
            case lexer::TOKEN_OPERATORA_ADD:
                rhs = binaryOperation(builder, lexer::TOKEN_OPERATORB_ADD,
                                      std::move(operands));
                break;
            case lexer::TOKEN_OPERATORA_SUB:
                rhs = binaryOperation(builder, lexer::TOKEN_OPERATORB_SUB,
                                      std::move(operands));
                break;
            case lexer::TOKEN_OPERATORA_MUL:
                rhs = binaryOperation(builder, lexer::TOKEN_OPERATORB_MUL,
                                      std::move(operands));
                break;
            case lexer::TOKEN_OPERATORA_DIV:
                rhs = binaryOperation(builder, lexer::TOKEN_OPERATORB_DIV,
                                      std::move(operands));
                break;
            case lexer::TOKEN_OPERATORA_MOD:
                rhs = binaryOperation(builder, lexer::TOKEN_OPERATORB_MOD,
                                      std::move(operands));
                break;
            default:
                return operationError(
                    "Unimplemented assignment operator for '{}': {}",
                    lhs->type->getDecoratedName(), op.get());
            }
            if(!rhs)
            {
                return nullptr;
            }
        }
        else
        {
            rhs = std::move(operands[1]);
        }

        builder.CreateStore(rhs->value, lhs->value);
        return rhs;
    }
    std::unique_ptr<TypedValue> IntegralTypeOperation::unaryOperation(
        llvm::IRBuilder<>& builder, core::lexer::TokenType op,
        std::vector<std::unique_ptr<TypedValue>> operands) const
    {
        assert(operands.size() == 1);

        auto ret = [&](llvm::Value* v) {
            return std::make_unique<TypedValue>(operands[0]->type, v);
        };

        switch(op.get())
        {
        case lexer::TOKEN_OPERATORU_PLUS:
        {
            bool castError = false, castNeeded = true;
            auto castop = operands[0]->type->defaultCast;
            auto t = type->typeTable->findDecorated("int");

            std::tie(castError, castNeeded, castop) =
                operands[0]->type->cast(Type::CAST, *type);
            if(castError)
            {
                return nullptr;
            }
            if(!castNeeded)
            {
                return ret(operands[0]->value);
            }

            return ret(builder.CreateCast(castop, operands[0]->value, t->type,
                                          "casttmp"));
        }
        case lexer::TOKEN_OPERATORU_MINUS:
            return ret(builder.CreateNeg(operands[0]->value, "negtmp"));
        case lexer::TOKEN_OPERATORU_NOT:
            return ret(builder.CreateNot(operands[0]->value, "nottmp"));
        default:
            return operationError("Unimplemented unary operator for '{}': '{}'",
                                  operands[0]->type->getDecoratedName(),
                                  op.get());
        }
    }
    std::unique_ptr<TypedValue> IntegralTypeOperation::binaryOperation(
        llvm::IRBuilder<>& builder, core::lexer::TokenType op,
        std::vector<std::unique_ptr<TypedValue>> operands) const
    {
        assert(operands.size() == 2);

        if(operands[0]->type != operands[1]->type)
        {
            return operationError("IntegralType binary operation operand types "
                                  "don't match: '{}' and '{}'",
                                  operands[0]->type->getDecoratedName(),
                                  operands[1]->type->getDecoratedName());
        }

        auto t = operands[0]->type;
        auto ret = [&](llvm::Value* v) {
            return std::make_unique<TypedValue>(t, v);
        };
        auto comp = [&](llvm::Value* v) {
            return std::make_unique<TypedValue>(
                type->typeTable->findDecorated("bool"), v);
        };
        switch(op.get())
        {
        case lexer::TOKEN_OPERATORB_ADD:
            return ret(builder.CreateAdd(operands[0]->value, operands[1]->value,
                                         "addtmp", true, true));
        case lexer::TOKEN_OPERATORB_SUB:
            return ret(builder.CreateSub(operands[0]->value, operands[1]->value,
                                         "subtmp", true, true));
        case lexer::TOKEN_OPERATORB_MUL:
            return ret(builder.CreateMul(operands[0]->value, operands[1]->value,
                                         "multmp", true, true));
        case lexer::TOKEN_OPERATORB_DIV:
            return ret(builder.CreateSDiv(operands[0]->value,
                                          operands[1]->value, "divtmp"));
        case lexer::TOKEN_OPERATORB_REM:
            return ret(builder.CreateSRem(operands[0]->value,
                                          operands[1]->value, "remtmp"));
        case lexer::TOKEN_OPERATORB_EQ:
            return comp(builder.CreateICmpEQ(operands[0]->value,
                                             operands[1]->value, "eqtmp"));
        case lexer::TOKEN_OPERATORB_NOTEQ:
            return comp(builder.CreateICmpNE(operands[0]->value,
                                             operands[1]->value, "neqtmp"));
        case lexer::TOKEN_OPERATORB_GREATER:
            return comp(builder.CreateICmpSGT(operands[0]->value,
                                              operands[1]->value, "gttmp"));
        case lexer::TOKEN_OPERATORB_GREATEQ:
            return comp(builder.CreateICmpSGE(operands[0]->value,
                                              operands[1]->value, "getmp"));
        case lexer::TOKEN_OPERATORB_LESS:
            return comp(builder.CreateICmpSLT(operands[0]->value,
                                              operands[1]->value, "lttmp"));
        case lexer::TOKEN_OPERATORB_LESSEQ:
            return comp(builder.CreateICmpSLE(operands[0]->value,
                                              operands[1]->value, "letmp"));
        default:
            return operationError("Unimplemented binary operator for '{}': {}",
                                  operands[0]->type->getDecoratedName(),
                                  op.get());
        }
    }
    std::unique_ptr<TypedValue> IntegralTypeOperation::arbitraryOperation(
        llvm::IRBuilder<>& builder, core::lexer::TokenType op,
        std::vector<std::unique_ptr<TypedValue>> operands) const
    {
        assert(operands.size() >= 1);
        return operationError(
            "No arbitrary-operand operations for '{}' are supported",
            operands[0]->type->getDecoratedName());
    }

    std::unique_ptr<TypedValue> CharTypeOperation::assignmentOperation(
        llvm::IRBuilder<>& builder, core::lexer::TokenType op,
        std::vector<std::unique_ptr<TypedValue>> operands) const
    {
        assert(operands.size() == 2);
        return operationError("No assignment operations for '{}' are supported",
                              operands[0]->type->getDecoratedName());
    }
    std::unique_ptr<TypedValue> CharTypeOperation::unaryOperation(
        llvm::IRBuilder<>& builder, core::lexer::TokenType op,
        std::vector<std::unique_ptr<TypedValue>> operands) const
    {
        assert(operands.size() == 1);
        return operationError("No unary operations for '{}' are supported",
                              operands[0]->type->getDecoratedName());
    }
    std::unique_ptr<TypedValue> CharTypeOperation::binaryOperation(
        llvm::IRBuilder<>& builder, core::lexer::TokenType op,
        std::vector<std::unique_ptr<TypedValue>> operands) const
    {
        assert(operands.size() == 2);
        return operationError("No binary operations for '{}' are supported",
                              operands[0]->type->getDecoratedName());
    }
    std::unique_ptr<TypedValue> CharTypeOperation::arbitraryOperation(
        llvm::IRBuilder<>& builder, core::lexer::TokenType op,
        std::vector<std::unique_ptr<TypedValue>> operands) const
    {
        assert(operands.size() >= 1);
        return operationError(
            "No arbitrary-operand operations for '{}' are supported",
            operands[0]->type->getDecoratedName());
    }

    std::unique_ptr<TypedValue> ByteTypeOperation::assignmentOperation(
        llvm::IRBuilder<>& builder, core::lexer::TokenType op,
        std::vector<std::unique_ptr<TypedValue>> operands) const
    {
        assert(operands.size() == 2);
        return operationError("No assignment operations for '{}' are supported",
                              operands[0]->type->getDecoratedName());
    }
    std::unique_ptr<TypedValue> ByteTypeOperation::unaryOperation(
        llvm::IRBuilder<>& builder, core::lexer::TokenType op,
        std::vector<std::unique_ptr<TypedValue>> operands) const
    {
        assert(operands.size() == 1);
        return operationError("No unary operations for '{}' are supported",
                              operands[0]->type->getDecoratedName());
    }
    std::unique_ptr<TypedValue> ByteTypeOperation::binaryOperation(
        llvm::IRBuilder<>& builder, core::lexer::TokenType op,
        std::vector<std::unique_ptr<TypedValue>> operands) const
    {
        assert(operands.size() == 2);
        return operationError("No binary operations for '{}' are supported",
                              operands[0]->type->getDecoratedName());
    }
    std::unique_ptr<TypedValue> ByteTypeOperation::arbitraryOperation(
        llvm::IRBuilder<>& builder, core::lexer::TokenType op,
        std::vector<std::unique_ptr<TypedValue>> operands) const
    {
        assert(operands.size() >= 1);
        return operationError(
            "No arbitrary-operand operations for '{}' are supported",
            operands[0]->type->getDecoratedName());
    }

    std::unique_ptr<TypedValue> FPTypeOperation::assignmentOperation(
        llvm::IRBuilder<>& builder, core::lexer::TokenType op,
        std::vector<std::unique_ptr<TypedValue>> operands) const
    {
        assert(operands.size() == 2);
        return operationError("No assignment operations for '{}' are supported",
                              operands[0]->type->getDecoratedName());
    }
    std::unique_ptr<TypedValue> FPTypeOperation::unaryOperation(
        llvm::IRBuilder<>& builder, core::lexer::TokenType op,
        std::vector<std::unique_ptr<TypedValue>> operands) const
    {
        assert(operands.size() == 1);
        return operationError("No unary operations for '{}' are supported",
                              operands[0]->type->getDecoratedName());
    }
    std::unique_ptr<TypedValue> FPTypeOperation::binaryOperation(
        llvm::IRBuilder<>& builder, core::lexer::TokenType op,
        std::vector<std::unique_ptr<TypedValue>> operands) const
    {
        assert(operands.size() == 2);
        return operationError("No binary operations for '{}' are supported",
                              operands[0]->type->getDecoratedName());
    }
    std::unique_ptr<TypedValue> FPTypeOperation::arbitraryOperation(
        llvm::IRBuilder<>& builder, core::lexer::TokenType op,
        std::vector<std::unique_ptr<TypedValue>> operands) const
    {
        assert(operands.size() >= 1);
        return operationError(
            "No arbitrary-operand operations for '{}' are supported",
            operands[0]->type->getDecoratedName());
    }

    std::unique_ptr<TypedValue> StringTypeOperation::assignmentOperation(
        llvm::IRBuilder<>& builder, core::lexer::TokenType op,
        std::vector<std::unique_ptr<TypedValue>> operands) const
    {
        assert(operands.size() == 2);
        return operationError("No assignment operations for '{}' are supported",
                              operands[0]->type->getDecoratedName());
    }
    std::unique_ptr<TypedValue> StringTypeOperation::unaryOperation(
        llvm::IRBuilder<>& builder, core::lexer::TokenType op,
        std::vector<std::unique_ptr<TypedValue>> operands) const
    {
        assert(operands.size() == 1);
        return operationError("No unary operations for '{}' are supported",
                              operands[0]->type->getDecoratedName());
    }
    std::unique_ptr<TypedValue> StringTypeOperation::binaryOperation(
        llvm::IRBuilder<>& builder, core::lexer::TokenType op,
        std::vector<std::unique_ptr<TypedValue>> operands) const
    {
        assert(operands.size() == 2);
        return operationError("No binary operations for '{}' are supported",
                              operands[0]->type->getDecoratedName());
    }
    std::unique_ptr<TypedValue> StringTypeOperation::arbitraryOperation(
        llvm::IRBuilder<>& builder, core::lexer::TokenType op,
        std::vector<std::unique_ptr<TypedValue>> operands) const
    {
        assert(operands.size() >= 1);
        return operationError(
            "No arbitrary-operand operations for '{}' are supported",
            operands[0]->type->getDecoratedName());
    }

    std::unique_ptr<TypedValue> FunctionTypeOperation::assignmentOperation(
        llvm::IRBuilder<>& builder, core::lexer::TokenType op,
        std::vector<std::unique_ptr<TypedValue>> operands) const
    {
        assert(operands.size() == 2);
        return operationError("No assignment operations for '{}' are supported",
                              operands[0]->type->getDecoratedName());
    }
    std::unique_ptr<TypedValue> FunctionTypeOperation::unaryOperation(
        llvm::IRBuilder<>& builder, core::lexer::TokenType op,
        std::vector<std::unique_ptr<TypedValue>> operands) const
    {
        assert(operands.size() == 1);
        return operationError("No unary operations for '{}' are supported",
                              operands[0]->type->getDecoratedName());
    }
    std::unique_ptr<TypedValue> FunctionTypeOperation::binaryOperation(
        llvm::IRBuilder<>& builder, core::lexer::TokenType op,
        std::vector<std::unique_ptr<TypedValue>> operands) const
    {
        assert(operands.size() == 2);
        return operationError("No binary operations for '{}' are supported",
                              operands[0]->type->getDecoratedName());
    }
    std::unique_ptr<TypedValue> FunctionTypeOperation::arbitraryOperation(
        llvm::IRBuilder<>& builder, core::lexer::TokenType op,
        std::vector<std::unique_ptr<TypedValue>> operands) const
    {
        assert(operands.size() >= 1);

        if(op != lexer::TOKEN_OPERATORC_CALL)
        {
            return operationError(
                "Unimplemented arbitrary-operand operator for '{}': '{}'",
                operands[0]->type->getDecoratedName(), op.get());
        }

        auto callee = operands[0].get();
        auto calleetype = dynamic_cast<FunctionType*>(callee->type);
        assert(calleetype);
        auto calleeval = llvm::cast<llvm::Function>(callee->value);

        size_t paramCount = operands.size() - 1;
        if(calleeval->arg_size() != paramCount)
        {
            return operationError(
                "Function call expects {} arguments, {} provided",
                calleeval->arg_size(), paramCount);
        }

        std::vector<llvm::Value*> args;
        for(size_t i = 1; i < paramCount + 1; ++i)
        {
            auto arg = operands[i].get();

            auto p = calleetype->params[i - 1];
            if(!arg->type->isSameOrImplicitlyCastable(*p))
            {
                return operationError("Invalid function call: Cannot convert "
                                      "parameter {} from {} to {}",
                                      i, arg->type->getDecoratedName(),
                                      p->getDecoratedName());
            }

            args.push_back(arg->value);
        }

        auto call = [&]() {
            if(calleetype->returnType->kind == Type::VOID)
            {
                auto c = llvm::CallInst::Create(calleeval->getFunctionType(),
                                                calleeval, args, "calltmp");
                builder.Insert(c);
                return c;
            }
            return builder.CreateCall(calleeval->getFunctionType(), calleeval,
                                      args, "calltmp");
        }();
        if(!call)
        {
            return nullptr;
        }

        auto retType = calleetype->returnType;
        return std::make_unique<TypedValue>(retType, call);
    }
} // namespace codegen
} // namespace code
