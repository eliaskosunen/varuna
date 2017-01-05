// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#include "core/ast/ASTControlStatement.h"
#include "core/ast/ASTExpression.h"
#include "core/ast/ASTFunctionStatement.h"
#include "core/ast/ASTLiteralExpression.h"
#include "core/ast/ASTNode.h"
#include "core/ast/ASTOperatorExpression.h"
#include "core/ast/ASTStatement.h"
#include "core/ast/FwdDecl.h"
#include "core/codegen/CodegenVisitor.h"

namespace core
{
namespace codegen
{
    std::unique_ptr<TypedValue> CodegenVisitor::visit(ast::ASTExpression* node)
    {
        codegenWarning("Unimplemented CodegenVisitor::visit({})",
                       node->nodeType.get());
        return nullptr;
    }
    std::unique_ptr<TypedValue> CodegenVisitor::visit(ast::ASTStatement* node)
    {
        codegenWarning("Unimplemented CodegenVisitor::visit({})",
                       node->nodeType.get());
        return nullptr;
    }
    std::unique_ptr<TypedValue> CodegenVisitor::visit(ast::ASTIfStatement* stmt)
    {
        auto cond = stmt->condition->accept(this);
        if(!cond)
        {
            return nullptr;
        }

        auto boolcond = [&]() -> llvm::Value* {
            if(!cond->type->isSized())
            {
                return codegenError(
                    "Unsized type cannot be used in if condition");
            }
            if(cond->type->kind == Type::BOOL)
            {
                return cond->value;
            }
            if(cond->type->isIntegral())
            {
                return builder.CreateICmpNE(
                    cond->value, llvm::ConstantInt::get(cond->type->type, 0),
                    "ifcond");
            }
            if(cond->type->isFloatingPoint())
            {
                return builder.CreateFCmpONE(
                    cond->value, llvm::ConstantFP::get(cond->type->type, 0.0),
                    "ifcond");
            }
            return nullptr;
        }();
        if(!boolcond)
        {
            return codegenError("Unable to compare if condition with 0");
        }

        llvm::Function* func = builder.GetInsertBlock()->getParent();

        llvm::BasicBlock* thenBB =
            llvm::BasicBlock::Create(context, "then", func);
        llvm::BasicBlock* elseBB = llvm::BasicBlock::Create(context, "else");
        llvm::BasicBlock* mergeBB = llvm::BasicBlock::Create(context, "ifcont");

        bool elseExists = stmt->elseBlock->nodeType != ast::ASTNode::EMPTY_STMT;

        if(elseExists)
        {
            builder.CreateCondBr(boolcond, thenBB, elseBB);
        }
        else
        {
            builder.CreateCondBr(boolcond, thenBB, mergeBB);
        }

        builder.SetInsertPoint(thenBB);

        auto thenV = stmt->ifBlock->accept(this);
        if(!thenV)
        {
            return nullptr;
        }

        builder.CreateBr(mergeBB);
        thenBB = builder.GetInsertBlock();

        if(elseExists)
        {
            func->getBasicBlockList().push_back(elseBB);
            builder.SetInsertPoint(elseBB);

            auto elseV = stmt->elseBlock->accept(this);
            if(!elseV)
            {
                return nullptr;
            }

            builder.CreateBr(mergeBB);
            elseBB = builder.GetInsertBlock();
        }

        func->getBasicBlockList().push_back(mergeBB);
        builder.SetInsertPoint(mergeBB);
        return createVoidVal(mergeBB);
    }
    std::unique_ptr<TypedValue>
    CodegenVisitor::visit(ast::ASTForStatement* node)
    {
        llvm::Function* func = builder.GetInsertBlock()->getParent();

        auto init = node->init->accept(this);
        if(!init)
        {
            return nullptr;
        }

        llvm::BasicBlock* loopBB =
            llvm::BasicBlock::Create(context, "loop", func);
        builder.CreateBr(loopBB);
        builder.SetInsertPoint(loopBB);

        if(!node->block->accept(this))
        {
            return nullptr;
        }

        llvm::BasicBlock* loopCondBB =
            llvm::BasicBlock::Create(context, "loopcond", func);
        builder.CreateBr(loopCondBB);
        builder.SetInsertPoint(loopCondBB);

        auto end = node->end->accept(this);
        if(!end)
        {
            return nullptr;
        }

        auto step = node->step->accept(this);
        if(!step)
        {
            return nullptr;
        }

        auto boolcond = [&]() -> llvm::Value* {
            if(!end->type->isSized())
            {
                return codegenError(
                    "Unsized type cannot be used in for condition");
            }
            if(end->type->kind == Type::BOOL)
            {
                return end->value;
            }
            if(end->type->isIntegral())
            {
                return builder.CreateICmpNE(
                    end->value, llvm::ConstantInt::get(end->type->type, 0),
                    "forendcond");
            }
            if(end->type->isFloatingPoint())
            {
                return builder.CreateFCmpONE(
                    end->value, llvm::ConstantFP::get(end->type->type, 0.0),
                    "forend");
            }
            return nullptr;
        }();
        if(!boolcond)
        {
            return codegenError("Unable to compare for end condition with 0");
        }

        llvm::BasicBlock* afterBB =
            llvm::BasicBlock::Create(context, "afterloop", func);
        builder.CreateCondBr(end->value, loopBB, afterBB);
        builder.SetInsertPoint(afterBB);

        return getTypedDummyValue();
    }
    std::unique_ptr<TypedValue>
    CodegenVisitor::visit(ast::ASTForeachStatement* node)
    {
        codegenWarning("Unimplemented CodegenVisitor::visit({})",
                       node->nodeType.get());
        return nullptr;
    }
    std::unique_ptr<TypedValue>
    CodegenVisitor::visit(ast::ASTWhileStatement* node)
    {
        codegenWarning("Unimplemented CodegenVisitor::visit({})",
                       node->nodeType.get());
        return nullptr;
    }
    std::unique_ptr<TypedValue>
    CodegenVisitor::visit(ast::ASTImportStatement* node)
    {
        codegenWarning("Unimplemented CodegenVisitor::visit({})",
                       node->nodeType.get());
        return nullptr;
    }
    std::unique_ptr<TypedValue>
    CodegenVisitor::visit(ast::ASTModuleStatement* node)
    {
        module->setModuleIdentifier(node->moduleName->value);
        return getTypedDummyValue();
    }

    std::unique_ptr<TypedValue> CodegenVisitor::visit(ast::ASTEmptyExpression*)
    {
        return getTypedDummyValue();
    }
    std::unique_ptr<TypedValue>
    CodegenVisitor::visit(ast::ASTIdentifierExpression* node)
    {
        codegenWarning("Unimplemented CodegenVisitor::visit({})",
                       node->nodeType.get());
        return nullptr;
    }
    std::unique_ptr<TypedValue>
    CodegenVisitor::visit(ast::ASTVariableRefExpression* expr)
    {
        auto var = variables.find(expr->value);
        if(var == variables.end())
        {
            var = globalVariables.find(expr->value);
            if(var == globalVariables.end())
            {
                return codegenError("Undefined variable: '{}'", expr->value);
            }
        }
        auto load = builder.CreateLoad(var->second.type->type,
                                       var->second.value, expr->value.c_str());
        if(!load)
        {
            return nullptr;
        }
        return std::make_unique<TypedValue>(var->second.type, load);
    }
    std::unique_ptr<TypedValue>
    CodegenVisitor::visit(ast::ASTCallExpression* expr)
    {
        // auto calleeExpr = expr->callee->accept(this);
        // auto calleeName = calleeExpr->getName().str();
        auto calleeName =
            static_cast<ast::ASTIdentifierExpression*>(expr->callee.get())
                ->value;
        llvm::Function* callee = findFunction(calleeName);
        if(!callee)
        {
            return nullptr;
        }

        if(callee->arg_size() != expr->params.size())
        {
            return codegenError("{} expects {} arguments, {} provided",
                                calleeName, callee->arg_size(),
                                expr->params.size());
        }

        std::vector<llvm::Value*> args;
        for(auto& arg : expr->params)
        {
            args.push_back(arg->accept(this)->value);
            if(!args.back())
            {
                return nullptr;
            }
        }

        auto call = builder.CreateCall(callee->getFunctionType(), callee, args,
                                       "calltmp");
        // auto call = builder.CreateCall(callee, args, "calltmp");
        auto type = findType(callee->getReturnType());
        if(!call || !type)
        {
            return nullptr;
        }
        return std::make_unique<TypedValue>(type, call);
    }
    std::unique_ptr<TypedValue>
    CodegenVisitor::visit(ast::ASTCastExpression* node)
    {
        auto castee = node->castee->accept(this);
        if(!castee)
        {
            return nullptr;
        }

        auto t = findType(node->type->value);
        if(!t)
        {
            return nullptr;
        }

        bool castError = false, castNeeded = true;
        llvm::Instruction::CastOps castop = t->defaultCast;
        std::tie(castError, castNeeded, castop) =
            castee->type->cast(Type::CAST, *t);
        if(castError)
        {
            return nullptr;
        }
        if(!castNeeded)
        {
            return castee;
        }
        auto cast =
            builder.CreateCast(castop, castee->value, t->type, "casttmp");
        return std::make_unique<TypedValue>(t, cast);
    }
    std::unique_ptr<TypedValue>
    CodegenVisitor::visit(ast::ASTVariableDefinitionExpression* expr)
    {
        llvm::Function* func = builder.GetInsertBlock()->getParent();

        auto t = findType(expr->type->value);
        if(!t)
        {
            return nullptr;
        }
        llvm::Type* type = t->type;

        if(!t->isSized())
        {
            return codegenError("Cannot create a variable of unsized type: {}",
                                t->name);
        }

        if(findType(expr->name->value, false))
        {
            return codegenError(
                "Cannot name variable as '{}': Reserved typename",
                expr->name->value);
        }

        auto init = expr->init.get();
        llvm::Value* initVal = nullptr;
        if(init->nodeType == ast::ASTNode::EMPTY_EXPR)
        {
            codegenWarning("Uninitialized variable: {}", expr->name->value);
            initVal = llvm::UndefValue::get(type);
            /*initVal = [type]() -> llvm::Value*
            {
                if(type->isIntegerTy())
                {
                    return llvm::ConstantInt::get(type, 0, false);
                }
                else if(type->isFloatingPointTy())
                {
                    return llvm::ConstantFP::get(type, 0.0);
                }
                else if(type->isPointerTy())
                {
                    return
            llvm::ConstantPointerNull::get(llvm::cast<llvm::PointerType>(type));
                }
                return llvm::UndefValue::get(type);
            }();*/
        }
        else
        {
            auto initExpr = init->accept(this);
            if(!initExpr)
            {
                return nullptr;
            }

            auto tname = initExpr->type->name;
            auto cast = checkTypedValue(std::move(initExpr), *t);
            if(!cast)
            {
                return codegenError(
                    "Invalid init expression: Cannot assign {} to {}", tname,
                    t->name);
            }
            initVal = cast->value;
        }
        if(!initVal)
        {
            return nullptr;
        }

        auto alloca = createEntryBlockAlloca(func, type, expr->name->value);
        builder.CreateStore(initVal, alloca);

        auto ty = findType(type);
        Variable var{alloca, ty, expr->name->value};
        variables.insert({expr->name->value, var});

        return std::make_unique<TypedValue>(ty, initVal);
    }
    std::unique_ptr<TypedValue>
    CodegenVisitor::visit(ast::ASTSubscriptExpression* node)
    {
        codegenWarning("Unimplemented CodegenVisitor::visit({})",
                       node->nodeType.get());
        return nullptr;
    }
    std::unique_ptr<TypedValue>
    CodegenVisitor::visit(ast::ASTSubscriptRangedExpression* node)
    {
        codegenWarning("Unimplemented CodegenVisitor::visit({})",
                       node->nodeType.get());
        return nullptr;
    }
    std::unique_ptr<TypedValue>
    CodegenVisitor::visit(ast::ASTMemberAccessExpression* node)
    {
        codegenWarning("Unimplemented CodegenVisitor::visit({})",
                       node->nodeType.get());
        return nullptr;
    }

    std::unique_ptr<TypedValue>
    CodegenVisitor::visit(ast::ASTFunctionParameter* node)
    {
        codegenWarning("Unimplemented CodegenVisitor::visit({})",
                       node->nodeType.get());
        return nullptr;
    }
    std::unique_ptr<TypedValue>
    CodegenVisitor::visit(ast::ASTFunctionPrototypeStatement* stmt)
    {
        std::vector<llvm::Type*> args;
        args.reserve(stmt->params.size());
        for(const auto& arg : stmt->params)
        {
            auto t = findType(arg->var->type->value);
            if(!t)
            {
                return nullptr;
            }
            args.push_back(t->type);
        }

        auto rt = findType(stmt->returnType->value);
        if(!rt)
        {
            return nullptr;
        }
        auto retType = rt->type;
        llvm::FunctionType* ft = llvm::FunctionType::get(retType, args, false);

        llvm::Function* f = llvm::Function::Create(
            ft, llvm::Function::ExternalLinkage, stmt->name->value, module);

        size_t i = 0;
        for(auto& arg : f->args())
        {
            arg.setName(stmt->params[i]->var->name->value);
        }

        return createVoidVal(f);
    }
    std::unique_ptr<TypedValue>
    CodegenVisitor::visit(ast::ASTFunctionDefinitionStatement* stmt)
    {
        auto name = stmt->proto->name->value;
        functionProtos[stmt->proto->name->value] = stmt->proto.get();
        auto func = findFunction(name);
        if(!func)
        {
            codegenWarning("Invalid function");
            return nullptr;
        }

        auto entry = llvm::BasicBlock::Create(context, "entry", func);
        builder.SetInsertPoint(entry);

        variables.clear();
        for(auto& arg : func->args())
        {
            auto alloca =
                createEntryBlockAlloca(func, arg.getType(), arg.getName());
            builder.CreateStore(&arg, alloca);
            auto t = findType(arg.getType());
            variables.insert(
                {arg.getName(), Variable{alloca, t, arg.getName()}});
        }

        if(!stmt->body->accept(this))
        {
            func->eraseFromParent();
            codegenWarning("Invalid function body");
            return nullptr;
        }

        auto backNodeType = [&]() -> ast::ASTNode::NodeType {
            auto& nodes = stmt->body->nodes;
            if(nodes.empty())
            {
                return ast::ASTNode::EMPTY_STMT;
            }
            return nodes.back()->nodeType;
        };
        if(func->getReturnType()->isVoidTy() &&
           backNodeType() != ast::ASTNode::RETURN_STMT)
        {
            builder.CreateRetVoid();
        }

#if USE_LLVM_FUNCTION_VERIFY
        if(!llvm::verifyFunction(*func))
        {
            codegenError("Function verification failed");
            util::logger->trace("Invalid function dump:");
            func->dump();
            func->eraseFromParent();
            return nullptr;
        }
#endif

        return createVoidVal(func);
    }
    std::unique_ptr<TypedValue>
    CodegenVisitor::visit(ast::ASTFunctionDeclarationStatement* stmt)
    {
        auto name = stmt->proto->name->value;
        functionProtos[stmt->proto->name->value] = stmt->proto.get();
        auto func = findFunction(name);
        if(!func)
        {
            codegenWarning("Invalid function");
            return nullptr;
        }
#if USE_LLVM_FUNCTION_VERIFY
        if(!llvm::verifyFunction(*func))
        {
            codegenError("Function verification failed");
            util::logger->trace("Invalid function dump:");
            func->dump();
            func->eraseFromParent();
            return nullptr;
        }
#endif
        return createVoidVal(func);
    }
    std::unique_ptr<TypedValue>
    CodegenVisitor::visit(ast::ASTReturnStatement* stmt)
    {
        if(stmt->returnValue->nodeType == ast::ASTNode::EMPTY_EXPR)
        {
            builder.CreateRetVoid();
            return getTypedDummyValue();
        }
        auto ret = stmt->returnValue->accept(this);
        if(!ret)
        {
            return nullptr;
        }
        auto f = getASTNodeFunction(stmt);
        auto checked =
            checkTypedValue(std::move(ret), *findType(f->returnType->value));
        if(!checked)
        {
            return nullptr;
        }

        builder.CreateRet(checked->value);
        return checked;
    }

    std::unique_ptr<TypedValue>
    CodegenVisitor::visit(ast::ASTIntegerLiteralExpression* expr)
    {
        auto t = findType(expr->type->value);
        auto val = llvm::ConstantInt::getSigned(t->type, expr->value);
        return std::make_unique<TypedValue>(t, val);
    }
    std::unique_ptr<TypedValue>
    CodegenVisitor::visit(ast::ASTFloatLiteralExpression* expr)
    {
        auto t = findType(expr->type->value);
        auto val = llvm::ConstantFP::get(t->type, expr->value);
        return std::make_unique<TypedValue>(t, val);
    }
    std::unique_ptr<TypedValue>
    CodegenVisitor::visit(ast::ASTStringLiteralExpression* expr)
    {
        auto str = createStringConstant(expr->value.c_str());
        return std::make_unique<TypedValue>(findType("string_t"), str);

        /*auto t = findType("string");
        if(!t)
        {
            return nullptr;
        }
        auto type = llvm::dyn_cast<llvm::StructType>(t->type);
        auto valtype = findType("int64");
        auto ret = llvm::ConstantStruct::get(type,
        {
            llvm::ConstantInt::get(valtype->type, str->getNumElements()),
            str
        });
        return std::make_unique<TypedValue>(valtype, ret);*/
        /*auto val = llvm::ConstantDataArray::getString(context, expr->value,
        false);
        if(!val)
        {
            return codegenError("Invalid string literal");
        }

        auto literal = llvm::dyn_cast<llvm::ConstantDataArray>(val);
        if(!literal)
        {
            return codegenError("Unable to cast String literal to
        ConstantDataArray");
        }

        auto t = findType("string");
        if(!t)
        {
            return nullptr;
        }
        auto type = llvm::dyn_cast<llvm::StructType>(t->type);
        auto valtype = findType("int64");
        auto ret = llvm::ConstantStruct::get(type,
        {
            llvm::ConstantInt::get(valtype->type, literal->getNumElements()),
            val
        });
        return std::make_unique<TypedValue>(valtype, ret);*/
    }
    std::unique_ptr<TypedValue>
    CodegenVisitor::visit(ast::ASTCharLiteralExpression* expr)
    {
        auto t = findType("char");
        auto val = llvm::ConstantInt::get(t->type, expr->value);
        return std::make_unique<TypedValue>(t, val);
    }
    std::unique_ptr<TypedValue>
    CodegenVisitor::visit(ast::ASTBoolLiteralExpression* expr)
    {
        auto t = findType("bool");
        auto val = llvm::ConstantInt::get(t->type, expr->value);
        return std::make_unique<TypedValue>(t, val);
    }
    std::unique_ptr<TypedValue>
    CodegenVisitor::visit(ast::ASTNoneLiteralExpression* node)
    {
        codegenWarning("Unimplemented CodegenVisitor::visit({})",
                       node->nodeType.get());
        return nullptr;
    }

    std::unique_ptr<TypedValue>
    CodegenVisitor::visit(ast::ASTBinaryOperationExpression* expr)
    {
        auto lhs = expr->lhs->accept(this);
        auto rhs_ = expr->rhs->accept(this);
        if(!lhs || !rhs_)
        {
            return nullptr;
        }

        auto rhs = checkTypedValue(std::move(rhs_), *lhs->type);
        if(!rhs)
        {
            return nullptr;
        }

        auto type = lhs->type;

        auto boolType = findType("bool");

        if(expr->oper == lexer::TOKEN_OPERATORB_EQ)
        {
            if(type->isIntegral())
            {
                return std::make_unique<TypedValue>(
                    boolType,
                    builder.CreateICmpEQ(lhs->value, rhs->value, "eqtmp"));
            }
            else if(type->isFloatingPoint())
            {
                return std::make_unique<TypedValue>(
                    boolType,
                    builder.CreateFCmpOEQ(lhs->value, rhs->value, "eqtmp"));
            }
            return codegenError("Cannot perform eq operation on {}",
                                type->name);
        }
        else if(expr->oper == lexer::TOKEN_OPERATORB_NOTEQ)
        {
            if(type->isIntegral())
            {
                return std::make_unique<TypedValue>(
                    boolType,
                    builder.CreateICmpNE(lhs->value, rhs->value, "noteqtmp"));
            }
            else if(type->isFloatingPoint())
            {
                return std::make_unique<TypedValue>(
                    boolType,
                    builder.CreateFCmpONE(lhs->value, rhs->value, "noteqtmp"));
            }
            return codegenError("Cannot perform noteq operation on {}",
                                type->name);
        }

        auto inst = [&]() -> llvm::Value* {
            switch(expr->oper.get())
            {
            case lexer::TOKEN_OPERATORB_ADD:
                if(type->isIntegral())
                {
                    return builder.CreateAdd(lhs->value, rhs->value, "addtmp");
                }
                else if(type->isFloatingPoint())
                {
                    return builder.CreateFAdd(lhs->value, rhs->value, "addtmp");
                }
                return codegenError("Cannot perform add operation on {}",
                                    type->name);
            case lexer::TOKEN_OPERATORB_SUB:
                if(type->isIntegral())
                {
                    return builder.CreateSub(lhs->value, rhs->value, "subtmp");
                }
                else if(type->isFloatingPoint())
                {
                    return builder.CreateFSub(lhs->value, rhs->value, "subtmp");
                }
                return codegenError("Cannot perform sub operation on {}",
                                    type->name);
            case lexer::TOKEN_OPERATORB_MUL:
                if(type->isIntegral())
                {
                    return builder.CreateMul(lhs->value, rhs->value, "multmp");
                }
                else if(type->isFloatingPoint())
                {
                    return builder.CreateFMul(lhs->value, rhs->value, "multmp");
                }
                return codegenError("Cannot perform mul operation on {}",
                                    type->name);
            case lexer::TOKEN_OPERATORB_DIV:
                if(type->isIntegral())
                {
                    return builder.CreateSDiv(lhs->value, rhs->value, "divtmp");
                }
                else if(type->isFloatingPoint())
                {
                    return builder.CreateFDiv(lhs->value, rhs->value, "divtmp");
                }
                return codegenError("Cannot perform div operation on {}",
                                    type->name);
            case lexer::TOKEN_OPERATORB_REM:
                if(type->isIntegral())
                {
                    return builder.CreateSRem(lhs->value, rhs->value, "remtmp");
                }
                else if(type->isFloatingPoint())
                {
                    return builder.CreateFRem(lhs->value, rhs->value, "remtmp");
                }
                return codegenError("Cannot perform rem operation on {}",
                                    type->name);

            default:
                return codegenError("Unimplemented binary operator");
            }
        }();
        if(!inst)
        {
            return nullptr;
        }
        return std::make_unique<TypedValue>(type, inst);
    }
    std::unique_ptr<TypedValue>
    CodegenVisitor::visit(ast::ASTUnaryOperationExpression* expr)
    {
        auto operand = expr->operand->accept(this);
        if(!operand)
        {
            return nullptr;
        }

        auto operandType = operand->type;

        if(expr->oper == lexer::TOKEN_OPERATORU_PLUS)
        {
            bool castError = false, castNeeded = true;
            llvm::Instruction::CastOps castop = operandType->defaultCast;
            auto type = findType("int");

            std::tie(castError, castNeeded, castop) =
                operandType->cast(Type::CAST, *type);
            if(castError)
            {
                return nullptr;
            }
            if(!castNeeded)
            {
                return std::make_unique<TypedValue>(operand->type,
                                                    operand->value);
            }

            auto cast = builder.CreateCast(castop, operand->value, type->type,
                                           "casttmp");
            return std::make_unique<TypedValue>(type, cast);
        }
        auto inst = [&]() -> llvm::Value* {
            switch(expr->oper.get())
            {
            case lexer::TOKEN_OPERATORU_PLUS:
                // Handled earlier
                llvm_unreachable("Unary + already handled");
            case lexer::TOKEN_OPERATORU_MINUS:
                if(operandType->isIntegral())
                {
                    return builder.CreateNeg(operand->value, "negtmp");
                }
                else if(operandType->isFloatingPoint())
                {
                    return builder.CreateFNeg(operand->value, "negtmp");
                }
                return codegenError(
                    "Type mismatch: Cannot perform neg on this type");
            case lexer::TOKEN_OPERATORU_NOT:
                if(operandType->isIntegral())
                {
                    return builder.CreateNot(operand->value, "nottmp");
                }
                else if(operandType->isFloatingPoint())
                {
                    return builder.CreateNot(operand->value, "nottmp");
                }
                return codegenError(
                    "Type mismatch: Cannot perform not on this type");
            default:
                return codegenError("Unimplemented unary operator"); // TODO
            }
        }();

        if(!inst)
        {
            return nullptr;
        }
        return std::make_unique<TypedValue>(operandType, inst);
    }
    std::unique_ptr<TypedValue>
    CodegenVisitor::visit(ast::ASTAssignmentOperationExpression* node)
    {
        if(node->oper != lexer::TOKEN_OPERATORA_SIMPLE)
        {
            codegenWarning(
                "Unimplemented CodegenVisitor::visit({}) for this operator",
                node->nodeType.get());
            return nullptr;
        }

        auto lhs =
            dynamic_cast<ast::ASTVariableRefExpression*>(node->lhs.get());
        if(!lhs)
        {
            return codegenError("'=' requires lhs to be a variable");
        }

        auto val = node->rhs->accept(this);
        if(!val)
        {
            codegenWarning("Assignment operator rhs codegen failed");
            return nullptr;
        }

        auto varit = variables.find(lhs->value);
        if(varit == variables.end())
        {
            return codegenError("Undefined variable '{}'", lhs->value);
        }

        llvm::Value* var = varit->second.value;
        if(!var)
        {
            return codegenError("Variable value is null");
        }
        builder.CreateStore(val->value, var);
        return val;
    }

    std::unique_ptr<TypedValue> CodegenVisitor::visit(ast::ASTEmptyStatement*)
    {
        return getTypedDummyValue();
    }
    std::unique_ptr<TypedValue>
    CodegenVisitor::visit(ast::ASTBlockStatement* stmt)
    {
        auto bb = llvm::BasicBlock::Create(context);

        if(stmt->nodes.empty())
        {
            codegenWarning("Empty block statement");
        }

        for(auto& child : stmt->nodes)
        {
            if(!child->accept(this))
            {
                codegenWarning("Block child codegen failed: {}",
                               child->nodeType);
                return nullptr;
            }
        }

        return createVoidVal(bb);
    }
    std::unique_ptr<TypedValue>
    CodegenVisitor::visit(ast::ASTWrappedExpressionStatement* stmt)
    {
        auto val = stmt->expr->accept(this);
        if(!val)
        {
            codegenWarning("Wrapped expression codegen failed: {}",
                           stmt->expr->nodeType);
        }
        return val;
    }
}
}
