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
#include "core/codegen/TypeOperation.h"

#define USE_LLVM_FUNCTION_VERIFY 0

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
        symbols.addBlock();

        auto cond = stmt->condition->accept(this);
        if(!cond)
        {
            return nullptr;
        }

        auto boolt = types.find("bool");
        assert(boolt);
        auto boolcond =
            cond->type->cast(builder, Type::IMPLICIT, cond->value, boolt);
        if(!boolcond)
        {
            return nullptr;
        }
        auto boolcondllvm = boolcond->value;

        llvm::Function* func = builder.GetInsertBlock()->getParent();

        llvm::BasicBlock* thenBB =
            llvm::BasicBlock::Create(context, "then", func);
        llvm::BasicBlock* elseBB = llvm::BasicBlock::Create(context, "else");
        llvm::BasicBlock* mergeBB = llvm::BasicBlock::Create(context, "ifcont");

        bool elseExists = stmt->elseBlock->nodeType != ast::ASTNode::EMPTY_STMT;

        if(elseExists)
        {
            builder.CreateCondBr(boolcondllvm, thenBB, elseBB);
        }
        else
        {
            builder.CreateCondBr(boolcondllvm, thenBB, mergeBB);
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

        symbols.removeTopBlock();

        func->getBasicBlockList().push_back(mergeBB);
        builder.SetInsertPoint(mergeBB);
        return createVoidVal(mergeBB);
    }
    std::unique_ptr<TypedValue>
    CodegenVisitor::visit(ast::ASTForStatement* node)
    {
        llvm::Function* func = builder.GetInsertBlock()->getParent();

        symbols.addBlock();

        auto loopInitBB = llvm::BasicBlock::Create(context, "loopinit", func);
        builder.CreateBr(loopInitBB);
        builder.SetInsertPoint(loopInitBB);

        auto init = node->init->accept(this);
        if(!init)
        {
            return nullptr;
        }

        auto loopCondBB = llvm::BasicBlock::Create(context, "loopcond", func);
        builder.SetInsertPoint(loopCondBB);

        auto cond = node->end->accept(this);
        if(!cond)
        {
            return nullptr;
        }

        auto boolt = types.find("bool");
        assert(boolt);
        auto boolcond =
            cond->type->cast(builder, Type::IMPLICIT, cond->value, boolt);
        if(!boolcond)
        {
            return nullptr;
        }

        auto loopBodyBB = llvm::BasicBlock::Create(context, "loopbody", func);
        builder.SetInsertPoint(loopBodyBB);

        auto body = node->block->accept(this);
        if(!body)
        {
            return nullptr;
        }

        auto loopStepBB = llvm::BasicBlock::Create(context, "loopstep", func);
        builder.SetInsertPoint(loopStepBB);

        auto step = node->step->accept(this);
        if(!step)
        {
            return nullptr;
        }

        auto loopEndBB = llvm::BasicBlock::Create(context, "loopend", func);

        builder.SetInsertPoint(loopInitBB);
        builder.CreateBr(loopCondBB);

        builder.SetInsertPoint(loopCondBB);
        builder.CreateCondBr(boolcond->value, loopBodyBB, loopEndBB);

        builder.SetInsertPoint(loopBodyBB);
        builder.CreateBr(loopStepBB);

        builder.SetInsertPoint(loopStepBB);
        builder.CreateBr(loopCondBB);

        builder.SetInsertPoint(loopEndBB);

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
        llvm::Function* func = builder.GetInsertBlock()->getParent();

        symbols.addBlock();

        auto condBB = llvm::BasicBlock::Create(context, "loopcond", func);
        builder.CreateBr(condBB);
        builder.SetInsertPoint(condBB);

        auto cond = node->condition->accept(this);
        if(!cond)
        {
            return nullptr;
        }

        auto boolt = types.find("bool");
        assert(boolt);
        auto boolcond =
            cond->type->cast(builder, Type::IMPLICIT, cond->value, boolt);
        if(!boolcond)
        {
            return nullptr;
        }

        auto bodyBB = llvm::BasicBlock::Create(context, "loopbody", func);
        builder.SetInsertPoint(bodyBB);

        auto body = node->block->accept(this);
        if(!body)
        {
            return nullptr;
        }

        auto endBB = llvm::BasicBlock::Create(context, "loopend", func);

        builder.SetInsertPoint(condBB);
        builder.CreateCondBr(boolcond->value, bodyBB, endBB);

        builder.SetInsertPoint(bodyBB);
        builder.CreateBr(condBB);

        builder.SetInsertPoint(endBB);

        symbols.removeTopBlock();

        return getTypedDummyValue();
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
        auto symbol = symbols.find(node->value);
        return std::make_unique<TypedValue>(symbol->getType(),
                                            symbol->value->value);
    }
    std::unique_ptr<TypedValue>
    CodegenVisitor::visit(ast::ASTVariableRefExpression* expr)
    {
        auto var = symbols.find(expr->value);
        if(!var)
        {
            return codegenError("Undefined variable: '{}'", expr->value);
        }
        auto load = builder.CreateLoad(var->getType()->type, var->value->value,
                                       expr->value.c_str());
        if(!load)
        {
            return nullptr;
        }
        return std::make_unique<TypedValue>(var->getType(), load);
    }
    std::unique_ptr<TypedValue>
    CodegenVisitor::visit(ast::ASTCastExpression* node)
    {
        auto castee = node->castee->accept(this);
        if(!castee)
        {
            return nullptr;
        }

        auto t = types.findDecorated(node->type->value);
        if(!t)
        {
            return nullptr;
        }

        return castee->type->cast(builder, Type::CAST, castee->value, t);
    }
    std::unique_ptr<TypedValue>
    CodegenVisitor::visit(ast::ASTVariableDefinitionExpression* expr)
    {
        // Initializer
        std::unique_ptr<TypedValue> init = nullptr;
        if(expr->init->nodeType != ast::ASTNode::EMPTY_EXPR)
        {
            init = expr->init->accept(this);
            if(!init)
            {
                return nullptr;
            }
        }

        // Infer type
        Type* type = nullptr;
        if(expr->typeInferred)
        {
            type = types.find(init->type->getName(),
                              expr->isMutable ? TypeTable::FIND_MUTABLE
                                              : TypeTable::FIND_DEFAULT);
        }
        else
        {
            type = types.find(expr->type->value, expr->isMutable
                                                     ? TypeTable::FIND_MUTABLE
                                                     : TypeTable::FIND_DEFAULT);
        }
        if(!type)
        {
            return nullptr;
        }

        // Type checks
        if(!type->isSized())
        {
            return codegenError("Cannot create a variable of unsized type: {}",
                                type->getDecoratedName());
        }
        if(types.isDefinedUndecorated(expr->name->value) != 0)
        {
            return codegenError(
                "Cannot name variable as '{}': Reserved typename",
                expr->name->value);
        }

        // No initializer:
        // Init as undefined
        if(!init)
        {
            if(expr->isMutable)
            {
                codegenWarning("Uninitialized variable: {}", expr->name->value);
            }
            else
            {
                codegenWarning(
                    "Useless variable: Uninitialized immutable variable: {}",
                    expr->name->value);
            }
            init = std::make_unique<TypedValue>(
                type, llvm::UndefValue::get(type->type));
            if(!init)
            {
                return nullptr;
            }
        }

        // Check init type
        if(!init->type->isSameOrImplicitlyCastable(builder, init->value, type))
        {
            return codegenError(
                "Invalid init expression: Cannot assign {} to {}",
                init->type->getDecoratedName(), type->getDecoratedName());
        }

        // Codegen
        llvm::Function* func = builder.GetInsertBlock()->getParent();
        auto alloca =
            createEntryBlockAlloca(func, type->type, expr->name->value);
        builder.CreateStore(init->value, alloca);

        auto var = std::make_unique<Symbol>(
            std::make_unique<TypedValue>(type, alloca), expr->name->value);
        symbols.getTop().insert(
            std::make_pair(expr->name->value, std::move(var)));

        return std::make_unique<TypedValue>(type, init->value);
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
        llvm::Function* func = builder.GetInsertBlock()->getParent();
        const auto var = node->var.get();

        auto type = types.findDecorated(var->type->value);
        if(!type)
        {
            return nullptr;
        }
        llvm::Type* llvmtype = type->type;

        if(!type->isSized())
        {
            return codegenError(
                "Function parameter cannot be of unsized type: {}",
                type->getName());
        }

        if(types.isDefinedUndecorated(var->name->value) != 0)
        {
            return codegenError(
                "Cannot name function parameter as '{}': Reserved typename",
                var->name->value);
        }

        bool store = false;
        llvm::Value* initVal = nullptr;
        if(var->init->nodeType != ast::ASTNode::EMPTY_EXPR)
        {
            auto initExpr = var->init->accept(this);
            if(!initExpr)
            {
                return nullptr;
            }

            auto typeName = initExpr->type->getDecoratedName();
            if(!initExpr->type->isSameOrImplicitlyCastable(
                   builder, initExpr->value, type))
            {
                return codegenError(
                    "Invalid init expression: Cannot assign {} to {}", typeName,
                    type->getDecoratedName());
            }
            initVal = initExpr->value;
        }

        auto alloca = createEntryBlockAlloca(func, llvmtype, var->name->value);
        if(store)
        {
            builder.CreateStore(initVal, alloca);
        }

        auto variable = std::make_unique<Symbol>(
            std::make_unique<TypedValue>(type, alloca), var->name->value);
        symbols.getTop().insert(
            std::make_pair(var->name->value, std::move(variable)));

        return std::make_unique<TypedValue>(type, alloca);
    }
    std::unique_ptr<TypedValue>
    CodegenVisitor::visit(ast::ASTFunctionPrototypeStatement* stmt)
    {
        std::vector<llvm::Type*> args;
        args.reserve(stmt->params.size());
        for(const auto& arg : stmt->params)
        {
            auto t = types.findDecorated(arg->var->type->value);
            if(!t)
            {
                return nullptr;
            }
            args.push_back(t->type);
        }

        auto rt = types.findDecorated(stmt->returnType->value);
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
        // Check for function type
        // If absent create one
        const auto proto = stmt->proto.get();
        const auto name = proto->name->value;
        auto returnType = types.findDecorated(proto->returnType->value);
        if(!returnType)
        {
            return nullptr;
        }
        std::vector<Type*> paramTypes;
        for(const auto& p : proto->params)
        {
            auto t = types.findDecorated(p->var->type->value);
            if(!t)
            {
                return nullptr;
            }
            paramTypes.push_back(t);
        }

        auto functionTypeBase = types.findDecorated(
            FunctionType::functionTypeToString(returnType, paramTypes), false);
        if(!functionTypeBase)
        {
            auto ft = std::make_unique<FunctionType>(&types, context, dbuilder,
                                                     returnType, paramTypes);
            functionTypeBase = ft.get();
            types.insertType(std::move(ft));
        }
        auto functionType = static_cast<FunctionType*>(functionTypeBase);

        // Check for declaration
        // If absent declare
        auto func = declareFunction(functionType, name, proto);
        if(!func)
        {
            return nullptr;
        }
        auto llvmfunc = llvm::cast<llvm::Function>(func->value->value);

        // Empty block statement, declaration
        if(stmt->body->nodes.empty())
        {
            return std::make_unique<TypedValue>(functionType, llvmfunc);
        }
        auto entry = llvm::BasicBlock::Create(context, "entry", llvmfunc);
        builder.SetInsertPoint(entry);

        symbols.addBlock();

        {
            auto llvmargs = llvmfunc->args();
            assert(stmt->proto->params.size() == llvmfunc->arg_size());
            auto argit = stmt->proto->params.begin();
            for(auto llvmargit = llvmargs.begin();
                argit != stmt->proto->params.end() &&
                llvmargit != llvmargs.end();
                ++argit, ++llvmargit)
            {
                auto& arg = *argit;

                auto vardef = arg->accept(this);
                if(!vardef)
                {
                    return nullptr;
                }

                auto& llvmarg = *llvmargit;
                auto alloca = vardef->value;
                builder.CreateStore(&llvmarg, alloca);
            }
        }

        if(!stmt->body->accept(this))
        {
            llvmfunc->eraseFromParent();
            codegenWarning("Function body code generation failed");
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
        if(llvmfunc->getReturnType()->isVoidTy() &&
           backNodeType() != ast::ASTNode::RETURN_STMT)
        {
            builder.CreateRetVoid();
        }

#if USE_LLVM_FUNCTION_VERIFY
        if(!llvm::verifyFunction(*llvmfunc))
        {
            codegenError("Function verification failed");
            util::logger->trace("Invalid function dump:");
            llvmfunc->dump();
            llvmfunc->eraseFromParent();
            return nullptr;
        }
#endif

        symbols.removeTopBlock();
        return std::make_unique<TypedValue>(functionType, llvmfunc);
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
        auto retType = types.findDecorated(f->returnType->value);
        assert(retType);
        if(!ret->type->isSameOrImplicitlyCastable(builder, ret->value, retType))
        {
            return nullptr;
        }

        builder.CreateRet(ret->value);
        return ret;
    }

    std::unique_ptr<TypedValue>
    CodegenVisitor::visit(ast::ASTIntegerLiteralExpression* expr)
    {
        auto t = types.findDecorated(expr->type->value);
        auto val = llvm::ConstantInt::getSigned(t->type, expr->value);
        return std::make_unique<TypedValue>(t, val);
    }
    std::unique_ptr<TypedValue>
    CodegenVisitor::visit(ast::ASTFloatLiteralExpression* expr)
    {
        auto t = types.findDecorated(expr->type->value);
        auto val = llvm::ConstantFP::get(t->type, expr->value);
        return std::make_unique<TypedValue>(t, val);
    }
    std::unique_ptr<TypedValue>
    CodegenVisitor::visit(ast::ASTStringLiteralExpression* expr)
    {
        auto str = createStringConstant(expr->value.c_str());
        return std::make_unique<TypedValue>(types.findDecorated("string_t"),
                                            str);

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
        auto t = types.findDecorated("char");
        auto val = llvm::ConstantInt::get(t->type, expr->value);
        return std::make_unique<TypedValue>(t, val);
    }
    std::unique_ptr<TypedValue>
    CodegenVisitor::visit(ast::ASTBoolLiteralExpression* expr)
    {
        auto t = types.findDecorated("bool");
        auto val = expr->value ? llvm::ConstantInt::getTrue(context)
                               : llvm::ConstantInt::getFalse(context);
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
        if(!lhs)
        {
            return nullptr;
        }

        auto rhs = expr->rhs->accept(this);
        if(!rhs)
        {
            return nullptr;
        }

        std::vector<std::unique_ptr<TypedValue>> operands;
        operands.push_back(std::move(lhs));
        operands.push_back(std::move(rhs));
        return operands.front()->type->getOperations()->binaryOperation(
            builder, expr->oper, std::move(operands));
    }
    std::unique_ptr<TypedValue>
    CodegenVisitor::visit(ast::ASTUnaryOperationExpression* expr)
    {

        auto operand = expr->operand->accept(this);
        if(!operand)
        {
            return nullptr;
        }

        std::vector<std::unique_ptr<TypedValue>> operands;
        operands.push_back(std::move(operand));
        return operands.front()->type->getOperations()->unaryOperation(
            builder, expr->oper, std::move(operands));
    }
    std::unique_ptr<TypedValue>
    CodegenVisitor::visit(ast::ASTAssignmentOperationExpression* node)
    {
        auto lhs = node->lhs->accept(this);
        if(!lhs)
        {
            return nullptr;
        }

        auto rhs = node->rhs->accept(this);
        if(!rhs)
        {
            return nullptr;
        }

        std::vector<std::unique_ptr<TypedValue>> operands;
        operands.push_back(std::move(lhs));
        operands.push_back(std::move(rhs));
        return operands.front()->type->getOperations()->assignmentOperation(
            builder, node->oper, std::move(operands));
    }
    std::unique_ptr<TypedValue>
    CodegenVisitor::visit(ast::ASTArbitraryOperationExpression* node)
    {
        std::vector<std::unique_ptr<TypedValue>> operands;

        for(auto& o : node->operands)
        {
            auto v = o->accept(this);
            if(!v)
            {
                return nullptr;
            }
            operands.push_back(std::move(v));
        }

        return operands.front()->type->getOperations()->arbitraryOperation(
            builder, node->oper, std::move(operands));
    }

    std::unique_ptr<TypedValue> CodegenVisitor::visit(ast::ASTEmptyStatement*)
    {
        return getTypedDummyValue();
    }
    std::unique_ptr<TypedValue>
    CodegenVisitor::visit(ast::ASTBlockStatement* stmt)
    {
        symbols.addBlock();

        for(auto& child : stmt->nodes)
        {
            if(!child->accept(this))
            {
                codegenWarning("Block child codegen failed: {}",
                               child->nodeType);

                // Not necessary,
                // execution will be stopped
                // symbols.removeTopBlock();
                return nullptr;
            }
        }

        symbols.removeTopBlock();
        return getTypedDummyValue();
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
} // namespace codegen
} // namespace core
