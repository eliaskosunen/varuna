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
std::unique_ptr<TypedValue> VoidTypeOperation::assignmentOperation(
    ast::ASTNode* node, llvm::IRBuilder<>& builder, util::OperatorType op,
    std::vector<TypedValue*> operands) const
{
    return operationError(
        node, "Invalid operation: Cannot make any operations on void");
}
std::unique_ptr<TypedValue> VoidTypeOperation::unaryOperation(
    ast::ASTNode* node, llvm::IRBuilder<>& builder, util::OperatorType op,
    std::vector<TypedValue*> operands) const
{
    return operationError(
        node, "Invalid operation: Cannot make any operations on void");
}
std::unique_ptr<TypedValue> VoidTypeOperation::binaryOperation(
    ast::ASTNode* node, llvm::IRBuilder<>& builder, util::OperatorType op,
    std::vector<TypedValue*> operands) const
{
    return operationError(
        node, "Invalid operation: Cannot make any operations on void");
}
std::unique_ptr<TypedValue> VoidTypeOperation::arbitraryOperation(
    ast::ASTNode* node, llvm::IRBuilder<>& builder, util::OperatorType op,
    std::vector<TypedValue*> operands) const
{
    return operationError(
        node, "Invalid operation: Cannot make any operations on void");
}

std::unique_ptr<TypedValue> IntegralTypeOperation::assignmentOperation(
    ast::ASTNode* node, llvm::IRBuilder<>& builder, util::OperatorType op,
    std::vector<TypedValue*> operands) const
{
    assert(operands.size() == 2);

    assert(operands[0]->cat != TypedValue::STMTVALUE);
    if(operands[0]->cat == TypedValue::RVALUE)
    {
        return operationError(node, "Cannot assign to an rvalue");
    }
    if(!operands[0]->isMutable)
    {
        return operationError(node, "Cannot assign to immutable lhs");
    }

    auto lhs = operands[0];
    assert(lhs);
    auto rhs = [&]() -> std::unique_ptr<TypedValue> {
        if(op != util::OPERATORA_SIMPLE)
        {
            switch(op.get())
            {
            case util::OPERATORA_ADD:
                return binaryOperation(node, builder, util::OPERATORB_ADD,
                                       operands);
            case util::OPERATORA_SUB:
                return binaryOperation(node, builder, util::OPERATORB_SUB,
                                       operands);
            case util::OPERATORA_MUL:
                return binaryOperation(node, builder, util::OPERATORB_MUL,
                                       operands);
            case util::OPERATORA_DIV:
                return binaryOperation(node, builder, util::OPERATORB_DIV,
                                       operands);
            case util::OPERATORA_MOD:
                return binaryOperation(node, builder, util::OPERATORB_MOD,
                                       operands);
            default:
                return operationError(
                    node, "Unsupported assignment operator for '{}': {}",
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
        return operationError(node, "Cannot assign '{}' to '{}'",
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
std::unique_ptr<TypedValue> IntegralTypeOperation::unaryOperation(
    ast::ASTNode* node, llvm::IRBuilder<>& builder, util::OperatorType op,
    std::vector<TypedValue*> operands) const
{
    assert(operands.size() == 1);

    auto ret = [&](llvm::Value* v) {
        return std::make_unique<TypedValue>(
            operands[0]->type, v, TypedValue::RVALUE, operands[0]->isMutable);
    };

    switch(op.get())
    {
    case util::OPERATORU_PLUS:
    {
        auto t = type->typeTable->findDecorated("int");
        assert(t);
        return operands[0]->type->cast(node, builder, Type::CAST, operands[0],
                                       t);
    }
    case util::OPERATORU_MINUS:
        return ret(builder.CreateNeg(operands[0]->value, "negtmp"));
    case util::OPERATORU_NOT:
        return ret(builder.CreateXor(operands[0]->value,
                                     static_cast<uint64_t>(-1), "compltmp"));
    default:
        return operationError(node, "Unsupported unary operator for '{}': '{}'",
                              operands[0]->type->getDecoratedName(), op.get());
    }
}
std::unique_ptr<TypedValue> IntegralTypeOperation::binaryOperation(
    ast::ASTNode* node, llvm::IRBuilder<>& builder, util::OperatorType op,
    std::vector<TypedValue*> operands) const
{
    assert(operands.size() == 2);

    if(operands[0]->type->basicInequal(*operands[1]->type))
    {
        return operationError(node,
                              "IntegralType binary operation operand types "
                              "don't match: '{}' and '{}'",
                              operands[0]->type->getDecoratedName(),
                              operands[1]->type->getDecoratedName());
    }

    auto t = operands[0]->type;
    auto ret = [&](llvm::Value* v) {
        return std::make_unique<TypedValue>(t, v, TypedValue::RVALUE,
                                            operands[0]->isMutable);
    };
    auto comp = [&](llvm::Value* v) {
        auto boolt = type->typeTable->findDecorated("bool");
        assert(boolt);
        return std::make_unique<TypedValue>(boolt, v, TypedValue::RVALUE,
                                            operands[0]->isMutable);
    };
    switch(op.get())
    {
    case util::OPERATORB_ADD:
        return ret(builder.CreateNSWAdd(operands[0]->value, operands[1]->value,
                                        "addtmp"));
    case util::OPERATORB_SUB:
        return ret(builder.CreateNSWSub(operands[0]->value, operands[1]->value,
                                        "subtmp"));
    case util::OPERATORB_MUL:
        return ret(builder.CreateNSWMul(operands[0]->value, operands[1]->value,
                                        "multmp"));
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
        return operationError(node, "Unsupported binary operator for '{}': {}",
                              operands[0]->type->getDecoratedName(), op.get());
    }
}
std::unique_ptr<TypedValue> IntegralTypeOperation::arbitraryOperation(
    ast::ASTNode* node, llvm::IRBuilder<>& builder, util::OperatorType op,
    std::vector<TypedValue*> operands) const
{
    assert(operands.size() >= 1);
    return operationError(
        node, "No arbitrary-operand operations for '{}' are supported",
        operands[0]->type->getDecoratedName());
}

std::unique_ptr<TypedValue> CharacterTypeOperation::assignmentOperation(
    ast::ASTNode* node, llvm::IRBuilder<>& builder, util::OperatorType op,
    std::vector<TypedValue*> operands) const
{
    assert(operands.size() == 2);

    assert(operands[0]->cat != TypedValue::STMTVALUE);
    if(operands[0]->cat == TypedValue::RVALUE)
    {
        return operationError(node, "Cannot assign to an rvalue");
    }
    if(!operands[0]->isMutable)
    {
        return operationError(node, "Cannot assign to immutable lhs");
    }

    auto lhs = operands[0];
    auto rhs = operands[1];
    assert(lhs);
    assert(rhs);

    if(op != util::OPERATORA_SIMPLE)
    {
        return operationError(node,
                              "Unsupported assignment operator for '{}': {}",
                              lhs->type->getDecoratedName(), op.get());
    }
    if(rhs->type->basicInequal(lhs->type))
    {
        return operationError(node, "Cannot assign '{}' to '{}'",
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
std::unique_ptr<TypedValue> CharacterTypeOperation::unaryOperation(
    ast::ASTNode* node, llvm::IRBuilder<>& builder, util::OperatorType op,
    std::vector<TypedValue*> operands) const
{
    assert(operands.size() == 1);
    return operationError(node, "No unary operations for '{}' are supported",
                          operands[0]->type->getDecoratedName());
}
std::unique_ptr<TypedValue> CharacterTypeOperation::binaryOperation(
    ast::ASTNode* node, llvm::IRBuilder<>& builder, util::OperatorType op,
    std::vector<TypedValue*> operands) const
{
    assert(operands.size() == 2);

    if(operands[0]->type->basicInequal(*operands[1]->type))
    {
        return operationError(node, "CharType binary operation operand types "
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
        return std::make_unique<TypedValue>(boolt, v, TypedValue::RVALUE,
                                            operands[0]->isMutable);
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
    return operationError(node, "Unsupported binary operator for '{}': {}",
                          operands[0]->type->getDecoratedName(), op.get());
}
std::unique_ptr<TypedValue> CharacterTypeOperation::arbitraryOperation(
    ast::ASTNode* node, llvm::IRBuilder<>& builder, util::OperatorType op,
    std::vector<TypedValue*> operands) const
{
    assert(operands.size() >= 1);
    return operationError(
        node, "No arbitrary-operand operations for '{}' are supported",
        operands[0]->type->getDecoratedName());
}

std::unique_ptr<TypedValue> BoolTypeOperation::assignmentOperation(
    ast::ASTNode* node, llvm::IRBuilder<>& builder, util::OperatorType op,
    std::vector<TypedValue*> operands) const
{
    assert(operands.size() == 2);

    assert(operands[0]->cat != TypedValue::STMTVALUE);
    if(operands[0]->cat == TypedValue::RVALUE)
    {
        return operationError(node, "Cannot assign to an rvalue");
    }
    if(!operands[0]->isMutable)
    {
        return operationError(node, "Cannot assign to immutable lhs");
    }

    auto lhs = operands[0];
    assert(lhs);
    auto rhs = [&]() -> std::unique_ptr<TypedValue> {
        if(op != util::OPERATORA_SIMPLE)
        {
            return operationError(
                node, "Unsupported assignment operator for '{}': {}",
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
std::unique_ptr<TypedValue> BoolTypeOperation::unaryOperation(
    ast::ASTNode* node, llvm::IRBuilder<>& builder, util::OperatorType op,
    std::vector<TypedValue*> operands) const
{
    assert(operands.size() == 1);

    auto ret = [&](llvm::Value* v) {
        return std::make_unique<TypedValue>(
            operands[0]->type, v, TypedValue::RVALUE, operands[0]->isMutable);
    };

    switch(op.get())
    {
    case util::OPERATORU_NOT:
        return ret(builder.CreateNot(operands[0]->value, "nottmp"));
    default:
        return operationError(node, "Unsupported unary operator for '{}': '{}'",
                              operands[0]->type->getDecoratedName(), op.get());
    }
}
std::unique_ptr<TypedValue> BoolTypeOperation::binaryOperation(
    ast::ASTNode* node, llvm::IRBuilder<>& builder, util::OperatorType op,
    std::vector<TypedValue*> operands) const
{
    assert(operands.size() == 2);

    if(operands[0]->type->basicInequal(*operands[1]->type))
    {
        return operationError(node, "BoolType binary operation operand types "
                                    "don't match: '{}' and '{}'",
                              operands[0]->type->getDecoratedName(),
                              operands[1]->type->getDecoratedName());
    }

    auto comp = [&](llvm::Value* v) {
        auto boolt = type->typeTable->findDecorated("bool");
        assert(boolt);
        return std::make_unique<TypedValue>(boolt, v, TypedValue::RVALUE,
                                            operands[0]->isMutable);
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
        return operationError(node, "Unsupported binary operator for '{}': {}",
                              operands[0]->type->getDecoratedName(), op.get());
    }
}
std::unique_ptr<TypedValue> BoolTypeOperation::arbitraryOperation(
    ast::ASTNode* node, llvm::IRBuilder<>& builder, util::OperatorType op,
    std::vector<TypedValue*> operands) const
{
    assert(operands.size() >= 1);
    return operationError(
        node, "No arbitrary-operand operations for '{}' are supported",
        operands[0]->type->getDecoratedName());
}

std::unique_ptr<TypedValue> ByteTypeOperation::assignmentOperation(
    ast::ASTNode* node, llvm::IRBuilder<>& builder, util::OperatorType op,
    std::vector<TypedValue*> operands) const
{
    assert(operands.size() == 2);
    return operationError(node,
                          "No assignment operations for '{}' are supported",
                          operands[0]->type->getDecoratedName());
}
std::unique_ptr<TypedValue> ByteTypeOperation::unaryOperation(
    ast::ASTNode* node, llvm::IRBuilder<>& builder, util::OperatorType op,
    std::vector<TypedValue*> operands) const
{
    assert(operands.size() == 1);
    return operationError(node, "No unary operations for '{}' are supported",
                          operands[0]->type->getDecoratedName());
}
std::unique_ptr<TypedValue> ByteTypeOperation::binaryOperation(
    ast::ASTNode* node, llvm::IRBuilder<>& builder, util::OperatorType op,
    std::vector<TypedValue*> operands) const
{
    assert(operands.size() == 2);
    return operationError(node, "No binary operations for '{}' are supported",
                          operands[0]->type->getDecoratedName());
}
std::unique_ptr<TypedValue> ByteTypeOperation::arbitraryOperation(
    ast::ASTNode* node, llvm::IRBuilder<>& builder, util::OperatorType op,
    std::vector<TypedValue*> operands) const
{
    assert(operands.size() >= 1);
    return operationError(
        node, "No arbitrary-operand operations for '{}' are supported",
        operands[0]->type->getDecoratedName());
}

std::unique_ptr<TypedValue> FPTypeOperation::assignmentOperation(
    ast::ASTNode* node, llvm::IRBuilder<>& builder, util::OperatorType op,
    std::vector<TypedValue*> operands) const
{
    assert(operands.size() == 2);

    assert(operands[0]->cat != TypedValue::STMTVALUE);
    if(operands[0]->cat == TypedValue::RVALUE)
    {
        return operationError(node, "Cannot assign to an rvalue");
    }
    if(!operands[0]->isMutable)
    {
        return operationError(node, "Cannot assign to immutable lhs");
    }

    auto lhs = operands[0];
    auto rhs = [&]() -> std::unique_ptr<TypedValue> {
        if(op != util::OPERATORA_SIMPLE)
        {
            switch(op.get())
            {
            case util::OPERATORA_ADD:
                return binaryOperation(node, builder, util::OPERATORB_ADD,
                                       operands);
            case util::OPERATORA_SUB:
                return binaryOperation(node, builder, util::OPERATORB_SUB,
                                       operands);
            case util::OPERATORA_MUL:
                return binaryOperation(node, builder, util::OPERATORB_MUL,
                                       operands);
            case util::OPERATORA_DIV:
                return binaryOperation(node, builder, util::OPERATORB_DIV,
                                       operands);
            case util::OPERATORA_MOD:
                return binaryOperation(node, builder, util::OPERATORB_MOD,
                                       operands);
            default:
                return operationError(
                    node, "Unsupported assignment operator for '{}': {}",
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
FPTypeOperation::unaryOperation(ast::ASTNode* node, llvm::IRBuilder<>& builder,
                                util::OperatorType op,
                                std::vector<TypedValue*> operands) const
{
    assert(operands.size() == 1);

    auto ret = [&](llvm::Value* v) {
        return std::make_unique<TypedValue>(
            operands[0]->type, v, TypedValue::RVALUE, operands[0]->isMutable);
    };

    switch(op.get())
    {
    case util::OPERATORU_PLUS:
    {
        auto t = type->typeTable->findDecorated("int");
        assert(t);
        return operands[0]->type->cast(node, builder, Type::CAST, operands[0],
                                       t);
    }
    case util::OPERATORU_MINUS:
        return ret(builder.CreateFNeg(operands[0]->value, "negtmp"));
    default:
        return operationError(node, "Unsupported unary operator for '{}': '{}'",
                              operands[0]->type->getDecoratedName(), op.get());
    }
}
std::unique_ptr<TypedValue>
FPTypeOperation::binaryOperation(ast::ASTNode* node, llvm::IRBuilder<>& builder,
                                 util::OperatorType op,
                                 std::vector<TypedValue*> operands) const
{
    assert(operands.size() == 2);

    if(operands[0]->type->basicInequal(*operands[1]->type))
    {
        return operationError(node, "FPType binary operation operand types "
                                    "don't match: '{}' and '{}'",
                              operands[0]->type->getDecoratedName(),
                              operands[1]->type->getDecoratedName());
    }

    auto t = operands[0]->type;
    auto ret = [&](llvm::Value* v) {
        return std::make_unique<TypedValue>(t, v, TypedValue::RVALUE,
                                            operands[0]->isMutable);
    };
    auto comp = [&](llvm::Value* v) {
        auto boolt = type->typeTable->findDecorated("bool");
        assert(boolt);
        return std::make_unique<TypedValue>(boolt, v, TypedValue::RVALUE,
                                            operands[0]->isMutable);
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
        return operationError(node, "Unsupported binary operator for '{}': {}",
                              operands[0]->type->getDecoratedName(), op.get());
    }
}
std::unique_ptr<TypedValue> FPTypeOperation::arbitraryOperation(
    ast::ASTNode* node, llvm::IRBuilder<>& builder, util::OperatorType op,
    std::vector<TypedValue*> operands) const
{
    assert(operands.size() >= 1);
    return operationError(
        node, "No arbitrary-operand operations for '{}' are supported",
        operands[0]->type->getDecoratedName());
}

std::unique_ptr<TypedValue> StringTypeOperation::assignmentOperation(
    ast::ASTNode* node, llvm::IRBuilder<>& builder, util::OperatorType op,
    std::vector<TypedValue*> operands) const
{
    assert(operands.size() == 2);

    assert(operands[0]->cat != TypedValue::STMTVALUE);
    if(operands[0]->cat == TypedValue::RVALUE)
    {
        return operationError(node, "Cannot assign to an rvalue");
    }
    if(!operands[0]->isMutable)
    {
        return operationError(node, "Cannot assign to immutable lhs");
    }

    auto lhs = operands[0];
    auto rhs = [&]() -> std::unique_ptr<TypedValue> {
        if(op != util::OPERATORA_SIMPLE)
        {
            return operationError(
                node, "Unsupported assignment operator for '{}': {}",
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

    auto lhsload = llvm::cast<llvm::LoadInst>(lhs->value);
    auto lhsval = lhsload->getPointerOperand();
    auto rhsval = rhs->value;

    builder.CreateStore(rhsval, lhsval);
    return std::make_unique<TypedValue>(*lhs);
}
std::unique_ptr<TypedValue> StringTypeOperation::unaryOperation(
    ast::ASTNode* node, llvm::IRBuilder<>& builder, util::OperatorType op,
    std::vector<TypedValue*> operands) const
{
    assert(operands.size() == 1);
    return operationError(node, "No unary operations for '{}' are supported",
                          operands[0]->type->getDecoratedName());
}
std::unique_ptr<TypedValue> StringTypeOperation::binaryOperation(
    ast::ASTNode* node, llvm::IRBuilder<>& builder, util::OperatorType op,
    std::vector<TypedValue*> operands) const
{
    assert(operands.size() == 2);
    return operationError(node, "No binary operations for '{}' are supported",
                          operands[0]->type->getDecoratedName());
}
std::unique_ptr<TypedValue> StringTypeOperation::arbitraryOperation(
    ast::ASTNode* node, llvm::IRBuilder<>& builder, util::OperatorType op,
    std::vector<TypedValue*> operands) const
{
    assert(operands.size() >= 1);
    return operationError(
        node, "No arbitrary-operand operations for '{}' are supported",
        operands[0]->type->getDecoratedName());
}

std::unique_ptr<TypedValue> CStringTypeOperation::assignmentOperation(
    ast::ASTNode* node, llvm::IRBuilder<>& builder, util::OperatorType op,
    std::vector<TypedValue*> operands) const
{
    assert(operands.size() == 2);

    assert(operands[0]->cat != TypedValue::STMTVALUE);
    if(operands[0]->cat == TypedValue::RVALUE)
    {
        return operationError(node, "Cannot assign to an rvalue");
    }
    if(!operands[0]->isMutable)
    {
        return operationError(node, "Cannot assign to immutable lhs");
    }

    auto lhs = operands[0];
    auto rhs = [&]() -> std::unique_ptr<TypedValue> {
        if(op != util::OPERATORA_SIMPLE)
        {
            return operationError(
                node, "Unsupported assignment operator for '{}': {}",
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

    auto lhsload = llvm::cast<llvm::LoadInst>(lhs->value);
    auto lhsval = lhsload->getPointerOperand();
    auto rhsval = rhs->value;

    builder.CreateStore(rhsval, lhsval);
    return std::make_unique<TypedValue>(*lhs);
}
std::unique_ptr<TypedValue> CStringTypeOperation::unaryOperation(
    ast::ASTNode* node, llvm::IRBuilder<>& builder, util::OperatorType op,
    std::vector<TypedValue*> operands) const
{
    assert(operands.size() == 1);
    return operationError(node, "No unary operations for '{}' are supported",
                          operands[0]->type->getDecoratedName());
}
std::unique_ptr<TypedValue> CStringTypeOperation::binaryOperation(
    ast::ASTNode* node, llvm::IRBuilder<>& builder, util::OperatorType op,
    std::vector<TypedValue*> operands) const
{
    assert(operands.size() == 2);
    return operationError(node, "No binary operations for '{}' are supported",
                          operands[0]->type->getDecoratedName());
}
std::unique_ptr<TypedValue> CStringTypeOperation::arbitraryOperation(
    ast::ASTNode* node, llvm::IRBuilder<>& builder, util::OperatorType op,
    std::vector<TypedValue*> operands) const
{
    assert(operands.size() >= 1);
    return operationError(
        node, "No arbitrary-operand operations for '{}' are supported",
        operands[0]->type->getDecoratedName());
}

std::unique_ptr<TypedValue> FunctionTypeOperation::assignmentOperation(
    ast::ASTNode* node, llvm::IRBuilder<>& builder, util::OperatorType op,
    std::vector<TypedValue*> operands) const
{
    assert(operands.size() == 2);
    return operationError(node,
                          "No assignment operations for '{}' are supported",
                          operands[0]->type->getDecoratedName());
}
std::unique_ptr<TypedValue> FunctionTypeOperation::unaryOperation(
    ast::ASTNode* node, llvm::IRBuilder<>& builder, util::OperatorType op,
    std::vector<TypedValue*> operands) const
{
    assert(operands.size() == 1);
    return operationError(node, "No unary operations for '{}' are supported",
                          operands[0]->type->getDecoratedName());
}
std::unique_ptr<TypedValue> FunctionTypeOperation::binaryOperation(
    ast::ASTNode* node, llvm::IRBuilder<>& builder, util::OperatorType op,
    std::vector<TypedValue*> operands) const
{
    assert(operands.size() == 2);
    return operationError(node, "No binary operations for '{}' are supported",
                          operands[0]->type->getDecoratedName());
}
std::unique_ptr<TypedValue> FunctionTypeOperation::arbitraryOperation(
    ast::ASTNode* node, llvm::IRBuilder<>& builder, util::OperatorType op,
    std::vector<TypedValue*> operands) const
{
    assert(operands.size() >= 1);

    if(op != util::OPERATORC_CALL)
    {
        return operationError(
            node, "Unsupported arbitrary-operand operator for '{}': '{}'",
            operands[0]->type->getDecoratedName(), op.get());
    }

    auto callee = operands[0];
    auto calleetype = static_cast<FunctionType*>(callee->type);
    assert(calleetype);
    auto calleeval = llvm::cast<llvm::Function>(callee->value);

    size_t paramCount = operands.size() - 1;
    if(calleeval->arg_size() != paramCount)
    {
        return operationError(node,
                              "Function call expects {} arguments, {} provided",
                              calleeval->arg_size(), paramCount);
    }

    std::vector<llvm::Value*> args;
    for(size_t i = 1; i < paramCount + 1; ++i)
    {
        auto arg = operands[i];

        auto p = calleetype->params[i - 1];
        if(!arg->type->isSameOrImplicitlyCastable(node, builder, arg, p))
        {
            return operationError(node, "Invalid function call: Cannot convert "
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
    assert(call);

    auto retType = calleetype->returnType;
    return std::make_unique<TypedValue>(retType, call, TypedValue::RVALUE,
                                        false);
}
} // namespace codegen
