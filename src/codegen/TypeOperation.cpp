// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#include "codegen/TypeOperation.h"
#include "codegen/Symbol.h"
#include "codegen/Type.h"
#include "codegen/TypeTable.h"
#include "codegen/TypedValue.h"

namespace codegen
{
std::unique_ptr<TypedValue>
VoidTypeOperation::assignmentOperation(llvm::IRBuilder<>& builder,
                                       util::OperatorType op,
                                       std::vector<TypedValue*> operands) const
{
    return operationError(
        "Invalid operation: Cannot make any operations on void");
}
std::unique_ptr<TypedValue>
VoidTypeOperation::unaryOperation(llvm::IRBuilder<>& builder,
                                  util::OperatorType op,
                                  std::vector<TypedValue*> operands) const
{
    return operationError(
        "Invalid operation: Cannot make any operations on void");
}
std::unique_ptr<TypedValue>
VoidTypeOperation::binaryOperation(llvm::IRBuilder<>& builder,
                                   util::OperatorType op,
                                   std::vector<TypedValue*> operands) const
{
    return operationError(
        "Invalid operation: Cannot make any operations on void");
}
std::unique_ptr<TypedValue>
VoidTypeOperation::arbitraryOperation(llvm::IRBuilder<>& builder,
                                      util::OperatorType op,
                                      std::vector<TypedValue*> operands) const
{
    return operationError(
        "Invalid operation: Cannot make any operations on void");
}

std::unique_ptr<TypedValue> IntegralTypeOperation::assignmentOperation(
    llvm::IRBuilder<>& builder, util::OperatorType op,
    std::vector<TypedValue*> operands) const
{
    assert(operands.size() == 2);

    if(!operands[0]->type->isMutable)
    {
        return operationError("Cannot assign to immutable lhs");
    }

    auto lhs = operands[0];
    assert(lhs);
    auto rhs = [&]() -> std::unique_ptr<TypedValue> {
        if(op != util::OPERATORA_SIMPLE)
        {
            switch(op.get())
            {
            case util::OPERATORA_ADD:
                return binaryOperation(builder, util::OPERATORB_ADD, operands);
            case util::OPERATORA_SUB:
                return binaryOperation(builder, util::OPERATORB_SUB, operands);
            case util::OPERATORA_MUL:
                return binaryOperation(builder, util::OPERATORB_MUL, operands);
            case util::OPERATORA_DIV:
                return binaryOperation(builder, util::OPERATORB_DIV, operands);
            case util::OPERATORA_MOD:
                return binaryOperation(builder, util::OPERATORB_MOD, operands);
            default:
                return operationError(
                    "Unsupported assignment operator for '{}': {}",
                    lhs->type->getDecoratedName(), op.get());
            }
        }
        else
        {
            return std::make_unique<TypedValue>(*operands[1]);
        }
    }();

    assert(lhs);
    if(!rhs)
    {
        return nullptr;
    }

    assert(lhs->type);
    assert(rhs->type);
    if(rhs->type->basicInequal(lhs->type))
    {
        return operationError("Cannot assign '{}' to '{}'",
                              rhs->type->getDecoratedName(),
                              lhs->type->getDecoratedName());
    }

    assert(lhs->value);
    assert(rhs->value);

    auto lhsload = llvm::dyn_cast<llvm::LoadInst>(lhs->value);
    assert(lhsload);
    auto lhsval = lhsload->getPointerOperand();
    assert(lhsval);

    auto rhsval = rhs->value;

    builder.CreateStore(rhsval, lhsval);
    return std::make_unique<TypedValue>(*lhs);
}
std::unique_ptr<TypedValue>
IntegralTypeOperation::unaryOperation(llvm::IRBuilder<>& builder,
                                      util::OperatorType op,
                                      std::vector<TypedValue*> operands) const
{
    assert(operands.size() == 1);

    auto ret = [&](llvm::Value* v) {
        return std::make_unique<TypedValue>(operands[0]->type, v);
    };

    switch(op.get())
    {
    case util::OPERATORU_PLUS:
    {
        auto t = type->typeTable->findDecorated("int");
        assert(t);
        return operands[0]->type->cast(builder, Type::CAST, operands[0]->value,
                                       t);
    }
    case util::OPERATORU_MINUS:
        return ret(builder.CreateNeg(operands[0]->value, "negtmp"));
    default:
        return operationError("Unsupported unary operator for '{}': '{}'",
                              operands[0]->type->getDecoratedName(), op.get());
    }
}
std::unique_ptr<TypedValue>
IntegralTypeOperation::binaryOperation(llvm::IRBuilder<>& builder,
                                       util::OperatorType op,
                                       std::vector<TypedValue*> operands) const
{
    assert(operands.size() == 2);

    if(operands[0]->type->basicInequal(*operands[1]->type))
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
        auto boolt = type->typeTable->findDecorated("bool");
        assert(boolt);
        return std::make_unique<TypedValue>(boolt, v);
    };
    switch(op.get())
    {
    case util::OPERATORB_ADD:
        return ret(builder.CreateAdd(operands[0]->value, operands[1]->value,
                                     "addtmp", true, true));
    case util::OPERATORB_SUB:
        return ret(builder.CreateSub(operands[0]->value, operands[1]->value,
                                     "subtmp", true, true));
    case util::OPERATORB_MUL:
        return ret(builder.CreateMul(operands[0]->value, operands[1]->value,
                                     "multmp", true, true));
    case util::OPERATORB_DIV:
        return ret(builder.CreateSDiv(operands[0]->value, operands[1]->value,
                                      "divtmp"));
    case util::OPERATORB_REM:
    case util::OPERATORB_MOD:
        return ret(builder.CreateSRem(operands[0]->value, operands[1]->value,
                                      "remtmp"));
    case util::OPERATORB_EQ:
        return comp(builder.CreateICmpEQ(operands[0]->value, operands[1]->value,
                                         "eqtmp"));
    case util::OPERATORB_NOTEQ:
        return comp(builder.CreateICmpNE(operands[0]->value, operands[1]->value,
                                         "neqtmp"));
    case util::OPERATORB_GREATER:
        return comp(builder.CreateICmpSGT(operands[0]->value,
                                          operands[1]->value, "gttmp"));
    case util::OPERATORB_GREATEQ:
        return comp(builder.CreateICmpSGE(operands[0]->value,
                                          operands[1]->value, "getmp"));
    case util::OPERATORB_LESS:
        return comp(builder.CreateICmpSLT(operands[0]->value,
                                          operands[1]->value, "lttmp"));
    case util::OPERATORB_LESSEQ:
        return comp(builder.CreateICmpSLE(operands[0]->value,
                                          operands[1]->value, "letmp"));
    default:
        return operationError("Unsupported binary operator for '{}': {}",
                              operands[0]->type->getDecoratedName(), op.get());
    }
}
std::unique_ptr<TypedValue> IntegralTypeOperation::arbitraryOperation(
    llvm::IRBuilder<>& builder, util::OperatorType op,
    std::vector<TypedValue*> operands) const
{
    assert(operands.size() >= 1);
    return operationError(
        "No arbitrary-operand operations for '{}' are supported",
        operands[0]->type->getDecoratedName());
}

std::unique_ptr<TypedValue> CharacterTypeOperation::assignmentOperation(
    llvm::IRBuilder<>& builder, util::OperatorType op,
    std::vector<TypedValue*> operands) const
{
    assert(operands.size() == 2);

    if(!operands[0]->type->isMutable)
    {
        return operationError("Cannot assign to immutable lhs");
    }

    auto lhs = operands[0];
    auto rhs = operands[1];
    assert(lhs);
    assert(rhs);

    if(op != util::OPERATORA_SIMPLE)
    {
        return operationError("Unsupported assignment operator for '{}': {}",
                              lhs->type->getDecoratedName(), op.get());
    }
    if(rhs->type->basicInequal(lhs->type))
    {
        return operationError("Cannot assign '{}' to '{}'",
                              rhs->type->getDecoratedName(),
                              lhs->type->getDecoratedName());
    }

    assert(lhs->value);
    assert(rhs->value);

    auto lhsload = llvm::dyn_cast<llvm::LoadInst>(lhs->value);
    assert(lhsload);
    auto lhsval = lhsload->getPointerOperand();
    assert(lhsval);

    auto rhsval = rhs->value;

    builder.CreateStore(rhsval, lhsval);
    return std::make_unique<TypedValue>(*lhs);
}
std::unique_ptr<TypedValue>
CharacterTypeOperation::unaryOperation(llvm::IRBuilder<>& builder,
                                       util::OperatorType op,
                                       std::vector<TypedValue*> operands) const
{
    assert(operands.size() == 1);
    return operationError("No unary operations for '{}' are supported",
                          operands[0]->type->getDecoratedName());
}
std::unique_ptr<TypedValue>
CharacterTypeOperation::binaryOperation(llvm::IRBuilder<>& builder,
                                        util::OperatorType op,
                                        std::vector<TypedValue*> operands) const
{
    assert(operands.size() == 2);

    if(operands[0]->type->basicInequal(*operands[1]->type))
    {
        return operationError("CharType binary operation operand types "
                              "don't match: '{}' and '{}'",
                              operands[0]->type->getDecoratedName(),
                              operands[1]->type->getDecoratedName());
    }

    /*auto t = operands[0]->type;
    auto ret = [&](llvm::Value* v) {
        return std::make_unique<TypedValue>(t, v);
    };*/
    auto comp = [&](llvm::Value* v) {
        auto boolt = type->typeTable->findDecorated("bool");
        assert(boolt);
        return std::make_unique<TypedValue>(boolt, v);
    };

    if(op == util::OPERATORB_EQ)
    {
        return comp(builder.CreateICmpEQ(operands[0]->value, operands[1]->value,
                                         "eqtmp"));
    }
    else if(op == util::OPERATORB_NOTEQ)
    {
        return comp(builder.CreateICmpNE(operands[0]->value, operands[1]->value,
                                         "eqtmp"));
    }
    return operationError("Unsupported binary operator for '{}': {}",
                          operands[0]->type->getDecoratedName(), op.get());
}
std::unique_ptr<TypedValue> CharacterTypeOperation::arbitraryOperation(
    llvm::IRBuilder<>& builder, util::OperatorType op,
    std::vector<TypedValue*> operands) const
{
    assert(operands.size() >= 1);
    return operationError(
        "No arbitrary-operand operations for '{}' are supported",
        operands[0]->type->getDecoratedName());
}

std::unique_ptr<TypedValue>
BoolTypeOperation::assignmentOperation(llvm::IRBuilder<>& builder,
                                       util::OperatorType op,
                                       std::vector<TypedValue*> operands) const
{
    assert(operands.size() == 2);

    if(!operands[0]->type->isMutable)
    {
        return operationError("Cannot assign to immutable lhs");
    }

    auto lhs = operands[0];
    assert(lhs);
    auto rhs = [&]() -> std::unique_ptr<TypedValue> {
        if(op != util::OPERATORA_SIMPLE)
        {
            return operationError(
                "Unsupported assignment operator for '{}': {}",
                lhs->type->getDecoratedName(), op.get());
        }
        else
        {
            return std::make_unique<TypedValue>(*operands[1]);
        }
    }();

    assert(lhs);
    if(!rhs)
    {
        return nullptr;
    }

    assert(lhs->value);
    assert(rhs->value);

    auto lhsload = llvm::dyn_cast<llvm::LoadInst>(lhs->value);
    assert(lhsload);
    auto lhsval = lhsload->getPointerOperand();
    assert(lhsval);

    auto rhsval = rhs->value;

    builder.CreateStore(rhsval, lhsval);
    return std::make_unique<TypedValue>(*lhs);
}
std::unique_ptr<TypedValue>
BoolTypeOperation::unaryOperation(llvm::IRBuilder<>& builder,
                                  util::OperatorType op,
                                  std::vector<TypedValue*> operands) const
{
    assert(operands.size() == 1);

    auto ret = [&](llvm::Value* v) {
        return std::make_unique<TypedValue>(operands[0]->type, v);
    };

    switch(op.get())
    {
    case util::OPERATORU_NOT:
        return ret(builder.CreateNot(operands[0]->value, "nottmp"));
    default:
        return operationError("Unsupported unary operator for '{}': '{}'",
                              operands[0]->type->getDecoratedName(), op.get());
    }
}
std::unique_ptr<TypedValue>
BoolTypeOperation::binaryOperation(llvm::IRBuilder<>& builder,
                                   util::OperatorType op,
                                   std::vector<TypedValue*> operands) const
{
    assert(operands.size() == 2);

    if(operands[0]->type->basicInequal(*operands[1]->type))
    {
        return operationError("BoolType binary operation operand types "
                              "don't match: '{}' and '{}'",
                              operands[0]->type->getDecoratedName(),
                              operands[1]->type->getDecoratedName());
    }

    auto comp = [&](llvm::Value* v) {
        auto boolt = type->typeTable->findDecorated("bool");
        assert(boolt);
        return std::make_unique<TypedValue>(boolt, v);
    };
    switch(op.get())
    {
    case util::OPERATORB_EQ:
        return comp(builder.CreateICmpEQ(operands[0]->value, operands[1]->value,
                                         "eqtmp"));
    case util::OPERATORB_NOTEQ:
        return comp(builder.CreateICmpNE(operands[0]->value, operands[1]->value,
                                         "neqtmp"));
    case util::OPERATORB_AND:
        return comp(builder.CreateAnd(operands[0]->value, operands[1]->value,
                                      "andtmp"));
    case util::OPERATORB_OR:
        return comp(
            builder.CreateOr(operands[0]->value, operands[1]->value, "ortmp"));
    default:
        return operationError("Unsupported binary operator for '{}': {}",
                              operands[0]->type->getDecoratedName(), op.get());
    }
}
std::unique_ptr<TypedValue>
BoolTypeOperation::arbitraryOperation(llvm::IRBuilder<>& builder,
                                      util::OperatorType op,
                                      std::vector<TypedValue*> operands) const
{
    assert(operands.size() >= 1);
    return operationError(
        "No arbitrary-operand operations for '{}' are supported",
        operands[0]->type->getDecoratedName());
}

std::unique_ptr<TypedValue>
ByteTypeOperation::assignmentOperation(llvm::IRBuilder<>& builder,
                                       util::OperatorType op,
                                       std::vector<TypedValue*> operands) const
{
    assert(operands.size() == 2);
    return operationError("No assignment operations for '{}' are supported",
                          operands[0]->type->getDecoratedName());
}
std::unique_ptr<TypedValue>
ByteTypeOperation::unaryOperation(llvm::IRBuilder<>& builder,
                                  util::OperatorType op,
                                  std::vector<TypedValue*> operands) const
{
    assert(operands.size() == 1);
    return operationError("No unary operations for '{}' are supported",
                          operands[0]->type->getDecoratedName());
}
std::unique_ptr<TypedValue>
ByteTypeOperation::binaryOperation(llvm::IRBuilder<>& builder,
                                   util::OperatorType op,
                                   std::vector<TypedValue*> operands) const
{
    assert(operands.size() == 2);
    return operationError("No binary operations for '{}' are supported",
                          operands[0]->type->getDecoratedName());
}
std::unique_ptr<TypedValue>
ByteTypeOperation::arbitraryOperation(llvm::IRBuilder<>& builder,
                                      util::OperatorType op,
                                      std::vector<TypedValue*> operands) const
{
    assert(operands.size() >= 1);
    return operationError(
        "No arbitrary-operand operations for '{}' are supported",
        operands[0]->type->getDecoratedName());
}

std::unique_ptr<TypedValue>
FPTypeOperation::assignmentOperation(llvm::IRBuilder<>& builder,
                                     util::OperatorType op,
                                     std::vector<TypedValue*> operands) const
{
    assert(operands.size() == 2);

    if(!operands[0]->type->isMutable)
    {
        return operationError("Cannot assign to immutable lhs");
    }

    auto lhs = operands[0];
    auto rhs = [&]() -> std::unique_ptr<TypedValue> {
        if(op != util::OPERATORA_SIMPLE)
        {
            switch(op.get())
            {
            case util::OPERATORA_ADD:
                return binaryOperation(builder, util::OPERATORB_ADD, operands);
            case util::OPERATORA_SUB:
                return binaryOperation(builder, util::OPERATORB_SUB, operands);
            case util::OPERATORA_MUL:
                return binaryOperation(builder, util::OPERATORB_MUL, operands);
            case util::OPERATORA_DIV:
                return binaryOperation(builder, util::OPERATORB_DIV, operands);
            case util::OPERATORA_MOD:
                return binaryOperation(builder, util::OPERATORB_MOD, operands);
            default:
                return operationError(
                    "Unsupported assignment operator for '{}': {}",
                    lhs->type->getDecoratedName(), op.get());
            }
        }
        else
        {
            return std::make_unique<TypedValue>(*operands[1]);
        }
    }();

    assert(lhs);
    if(!rhs)
    {
        return nullptr;
    }

    auto lhsload = llvm::cast<llvm::LoadInst>(lhs->value);
    auto lhsval = lhsload->getPointerOperand();
    auto rhsval = rhs->value;

    builder.CreateStore(rhsval, lhsval);
    return std::make_unique<TypedValue>(*lhs);
}
std::unique_ptr<TypedValue>
FPTypeOperation::unaryOperation(llvm::IRBuilder<>& builder,
                                util::OperatorType op,
                                std::vector<TypedValue*> operands) const
{
    assert(operands.size() == 1);

    auto ret = [&](llvm::Value* v) {
        return std::make_unique<TypedValue>(operands[0]->type, v);
    };

    switch(op.get())
    {
    case util::OPERATORU_PLUS:
    {
        auto t = type->typeTable->findDecorated("int");
        assert(t);
        return operands[0]->type->cast(builder, Type::CAST, operands[0]->value,
                                       t);
    }
    case util::OPERATORU_MINUS:
        return ret(builder.CreateFNeg(operands[0]->value, "negtmp"));
    default:
        return operationError("Unsupported unary operator for '{}': '{}'",
                              operands[0]->type->getDecoratedName(), op.get());
    }
}
std::unique_ptr<TypedValue>
FPTypeOperation::binaryOperation(llvm::IRBuilder<>& builder,
                                 util::OperatorType op,
                                 std::vector<TypedValue*> operands) const
{
    assert(operands.size() == 2);

    if(operands[0]->type->basicInequal(*operands[1]->type))
    {
        return operationError("FPType binary operation operand types "
                              "don't match: '{}' and '{}'",
                              operands[0]->type->getDecoratedName(),
                              operands[1]->type->getDecoratedName());
    }

    auto t = operands[0]->type;
    auto ret = [&](llvm::Value* v) {
        return std::make_unique<TypedValue>(t, v);
    };
    auto comp = [&](llvm::Value* v) {
        auto boolt = type->typeTable->findDecorated("bool");
        assert(boolt);
        return std::make_unique<TypedValue>(boolt, v);
    };
    switch(op.get())
    {
    case util::OPERATORB_ADD:
        return ret(builder.CreateFAdd(operands[0]->value, operands[1]->value,
                                      "addtmp"));
    case util::OPERATORB_SUB:
        return ret(builder.CreateFSub(operands[0]->value, operands[1]->value,
                                      "subtmp"));
    case util::OPERATORB_MUL:
        return ret(builder.CreateFMul(operands[0]->value, operands[1]->value,
                                      "multmp"));
    case util::OPERATORB_DIV:
        return ret(builder.CreateFDiv(operands[0]->value, operands[1]->value,
                                      "divtmp"));
    case util::OPERATORB_REM:
    case util::OPERATORB_MOD:
        return ret(builder.CreateFRem(operands[0]->value, operands[1]->value,
                                      "remtmp"));
    case util::OPERATORB_EQ:
        return comp(builder.CreateFCmpOEQ(operands[0]->value,
                                          operands[1]->value, "eqtmp"));
    case util::OPERATORB_NOTEQ:
        return comp(builder.CreateFCmpONE(operands[0]->value,
                                          operands[1]->value, "neqtmp"));
    case util::OPERATORB_GREATER:
        return comp(builder.CreateFCmpOGT(operands[0]->value,
                                          operands[1]->value, "gttmp"));
    case util::OPERATORB_GREATEQ:
        return comp(builder.CreateFCmpOGE(operands[0]->value,
                                          operands[1]->value, "getmp"));
    case util::OPERATORB_LESS:
        return comp(builder.CreateFCmpOLT(operands[0]->value,
                                          operands[1]->value, "lttmp"));
    case util::OPERATORB_LESSEQ:
        return comp(builder.CreateFCmpOLE(operands[0]->value,
                                          operands[1]->value, "letmp"));
    default:
        return operationError("Unsupported binary operator for '{}': {}",
                              operands[0]->type->getDecoratedName(), op.get());
    }
}
std::unique_ptr<TypedValue>
FPTypeOperation::arbitraryOperation(llvm::IRBuilder<>& builder,
                                    util::OperatorType op,
                                    std::vector<TypedValue*> operands) const
{
    assert(operands.size() >= 1);
    return operationError(
        "No arbitrary-operand operations for '{}' are supported",
        operands[0]->type->getDecoratedName());
}

std::unique_ptr<TypedValue> StringTypeOperation::assignmentOperation(
    llvm::IRBuilder<>& builder, util::OperatorType op,
    std::vector<TypedValue*> operands) const
{
    assert(operands.size() == 2);
    return operationError("No assignment operations for '{}' are supported",
                          operands[0]->type->getDecoratedName());
}
std::unique_ptr<TypedValue>
StringTypeOperation::unaryOperation(llvm::IRBuilder<>& builder,
                                    util::OperatorType op,
                                    std::vector<TypedValue*> operands) const
{
    assert(operands.size() == 1);
    return operationError("No unary operations for '{}' are supported",
                          operands[0]->type->getDecoratedName());
}
std::unique_ptr<TypedValue>
StringTypeOperation::binaryOperation(llvm::IRBuilder<>& builder,
                                     util::OperatorType op,
                                     std::vector<TypedValue*> operands) const
{
    assert(operands.size() == 2);
    return operationError("No binary operations for '{}' are supported",
                          operands[0]->type->getDecoratedName());
}
std::unique_ptr<TypedValue>
StringTypeOperation::arbitraryOperation(llvm::IRBuilder<>& builder,
                                        util::OperatorType op,
                                        std::vector<TypedValue*> operands) const
{
    assert(operands.size() >= 1);
    return operationError(
        "No arbitrary-operand operations for '{}' are supported",
        operands[0]->type->getDecoratedName());
}

std::unique_ptr<TypedValue> FunctionTypeOperation::assignmentOperation(
    llvm::IRBuilder<>& builder, util::OperatorType op,
    std::vector<TypedValue*> operands) const
{
    assert(operands.size() == 2);
    return operationError("No assignment operations for '{}' are supported",
                          operands[0]->type->getDecoratedName());
}
std::unique_ptr<TypedValue>
FunctionTypeOperation::unaryOperation(llvm::IRBuilder<>& builder,
                                      util::OperatorType op,
                                      std::vector<TypedValue*> operands) const
{
    assert(operands.size() == 1);
    return operationError("No unary operations for '{}' are supported",
                          operands[0]->type->getDecoratedName());
}
std::unique_ptr<TypedValue>
FunctionTypeOperation::binaryOperation(llvm::IRBuilder<>& builder,
                                       util::OperatorType op,
                                       std::vector<TypedValue*> operands) const
{
    assert(operands.size() == 2);
    return operationError("No binary operations for '{}' are supported",
                          operands[0]->type->getDecoratedName());
}
std::unique_ptr<TypedValue> FunctionTypeOperation::arbitraryOperation(
    llvm::IRBuilder<>& builder, util::OperatorType op,
    std::vector<TypedValue*> operands) const
{
    assert(operands.size() >= 1);

    if(op != util::OPERATORC_CALL)
    {
        return operationError(
            "Unsupported arbitrary-operand operator for '{}': '{}'",
            operands[0]->type->getDecoratedName(), op.get());
    }

    auto callee = operands[0];
    auto calleetype = dynamic_cast<FunctionType*>(callee->type);
    assert(calleetype);
    auto calleeval = llvm::cast<llvm::Function>(callee->value);

    size_t paramCount = operands.size() - 1;
    if(calleeval->arg_size() != paramCount)
    {
        return operationError("Function call expects {} arguments, {} provided",
                              calleeval->arg_size(), paramCount);
    }

    std::vector<llvm::Value*> args;
    for(size_t i = 1; i < paramCount + 1; ++i)
    {
        auto arg = operands[i];

        auto p = calleetype->params[i - 1];
        if(!arg->type->isSameOrImplicitlyCastable(builder, arg->value, p))
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
        return builder.CreateCall(calleeval->getFunctionType(), calleeval, args,
                                  "calltmp");
    }();
    if(!call)
    {
        return nullptr;
    }

    auto retType = calleetype->returnType;
    return std::make_unique<TypedValue>(retType, call);
}
} // namespace codegen
