// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#include "ast/ASTControlStatement.h"
#include "ast/ASTExpression.h"
#include "ast/ASTFunctionStatement.h"
#include "ast/ASTLiteralExpression.h"
#include "ast/ASTNode.h"
#include "ast/ASTOperatorExpression.h"
#include "ast/ASTStatement.h"
#include "ast/FwdDecl.h"
#include "codegen/CodegenVisitor.h"
#include "codegen/TypeOperation.h"

#define USE_LLVM_FUNCTION_VERIFY 0

namespace codegen
{
std::unique_ptr<TypedValue> CodegenVisitor::visit(ast::ASTExpression* node)
{
    codegenWarning(node, "Unimplemented CodegenVisitor::visit({})",
                   node->nodeType.get());
    return nullptr;
}
std::unique_ptr<TypedValue> CodegenVisitor::visit(ast::ASTStatement* node)
{
    codegenWarning(node, "Unimplemented CodegenVisitor::visit({})",
                   node->nodeType.get());
    return nullptr;
}
std::unique_ptr<TypedValue> CodegenVisitor::visit(ast::ASTIfStatement* node)
{
    emitDebugLocation(node);

    symbols.addBlock();

    if(info.emitDebug)
    {
        auto d = dbuilder.createLexicalBlock(dblocks.back(), dfile,
                                             node->loc.line, 0);
        dblocks.push_back(d);
    }

    auto cond = node->condition->accept(this);
    if(!cond)
    {
        return nullptr;
    }

    auto boolt = types.find("bool");
    assert(boolt);
    auto boolcond = cond->type->cast(node->condition.get(), builder,
                                     Type::IMPLICIT, cond->value, boolt);
    if(!boolcond)
    {
        return nullptr;
    }
    auto boolcondllvm = boolcond->value;

    llvm::Function* func = builder.GetInsertBlock()->getParent();

    auto thenBB = llvm::BasicBlock::Create(context, "then", func);
    auto elseBB = llvm::BasicBlock::Create(context, "else");
    auto mergeBB = llvm::BasicBlock::Create(context, "ifcont");

    bool elseExists = node->elseBlock->nodeType != ast::ASTNode::EMPTY_STMT;

    if(elseExists)
    {
        builder.CreateCondBr(boolcondllvm, thenBB, elseBB);
    }
    else
    {
        builder.CreateCondBr(boolcondllvm, thenBB, mergeBB);
    }

    builder.SetInsertPoint(thenBB);

    auto thenV = node->ifBlock->accept(this);
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

        auto elseV = node->elseBlock->accept(this);
        if(!elseV)
        {
            return nullptr;
        }

        builder.CreateBr(mergeBB);
        elseBB = builder.GetInsertBlock();
    }

    if(info.emitDebug)
    {
        dblocks.pop_back();
    }
    symbols.removeTopBlock();

    func->getBasicBlockList().push_back(mergeBB);
    builder.SetInsertPoint(mergeBB);
    return createVoidVal(mergeBB);
}
std::unique_ptr<TypedValue> CodegenVisitor::visit(ast::ASTForStatement* node)
{
    llvm::Function* func = builder.GetInsertBlock()->getParent();

    emitDebugLocation(node);
    symbols.addBlock();

    if(info.emitDebug)
    {
        auto d = dbuilder.createLexicalBlock(dblocks.back(), dfile,
                                             node->loc.line, 0);
        dblocks.push_back(d);
    }

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
    auto boolcond = cond->type->cast(node->end.get(), builder, Type::IMPLICIT,
                                     cond->value, boolt);
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

    auto bodyInsertBlock = builder.GetInsertBlock();
    auto bodyInsertPoint = builder.GetInsertPoint();

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

    builder.SetInsertPoint(bodyInsertBlock, bodyInsertPoint);
    builder.CreateBr(loopStepBB);

    builder.SetInsertPoint(loopStepBB);
    builder.CreateBr(loopCondBB);

    builder.SetInsertPoint(loopEndBB);

    if(info.emitDebug)
    {
        dblocks.pop_back();
    }
    symbols.removeTopBlock();

    return getTypedDummyValue();
}
std::unique_ptr<TypedValue>
CodegenVisitor::visit(ast::ASTForeachStatement* node)
{
    codegenWarning(node, "Unimplemented CodegenVisitor::visit({})",
                   node->nodeType.get());
    return nullptr;
}
std::unique_ptr<TypedValue> CodegenVisitor::visit(ast::ASTWhileStatement* node)
{
    llvm::Function* func = builder.GetInsertBlock()->getParent();

    emitDebugLocation(node);
    symbols.addBlock();

    if(info.emitDebug)
    {
        auto d = dbuilder.createLexicalBlock(dblocks.back(), dfile,
                                             node->loc.line, 0);
        dblocks.push_back(d);
    }

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
    auto boolcond = cond->type->cast(node->condition.get(), builder,
                                     Type::IMPLICIT, cond->value, boolt);
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

    auto bodyInsertBlock = builder.GetInsertBlock();
    auto bodyInsertPoint = builder.GetInsertPoint();

    auto endBB = llvm::BasicBlock::Create(context, "loopend", func);

    builder.SetInsertPoint(condBB);
    builder.CreateCondBr(boolcond->value, bodyBB, endBB);

    builder.SetInsertPoint(bodyInsertBlock, bodyInsertPoint);
    builder.CreateBr(condBB);

    builder.SetInsertPoint(endBB);

    if(info.emitDebug)
    {
        dblocks.pop_back();
    }
    symbols.removeTopBlock();

    return getTypedDummyValue();
}
std::unique_ptr<TypedValue> CodegenVisitor::visit(ast::ASTImportStatement* node)
{
    codegenWarning(node, "Unimplemented CodegenVisitor::visit({})",
                   node->nodeType.get());
    return nullptr;
}
std::unique_ptr<TypedValue> CodegenVisitor::visit(ast::ASTModuleStatement* node)
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
    if(!symbol)
    {
        util::logger->trace("Node loc: '{}'", node->loc.toString());
        return codegenError(node, "Undefined symbol: '{}'", node->value);
    }
    return std::make_unique<TypedValue>(symbol->getType(),
                                        symbol->value->value);
}
std::unique_ptr<TypedValue>
CodegenVisitor::visit(ast::ASTVariableRefExpression* node)
{
    emitDebugLocation(node);

    auto var = symbols.find(node->value);
    if(!var)
    {
        return codegenError(node, "Undefined variable: '{}'", node->value);
    }
    auto load = builder.CreateLoad(var->getType()->type, var->value->value,
                                   node->value.c_str());
    if(!load)
    {
        return nullptr;
    }
    return std::make_unique<TypedValue>(var->getType(), load);
}
std::unique_ptr<TypedValue> CodegenVisitor::visit(ast::ASTCastExpression* node)
{
    emitDebugLocation(node);

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

    return castee->type->cast(node, builder, Type::CAST, castee->value, t);
}
std::unique_ptr<TypedValue>
CodegenVisitor::visit(ast::ASTVariableDefinitionExpression* node)
{
    Type* type;
    std::unique_ptr<TypedValue> init;
    std::tie(type, init) = inferVariableDefType(node);

    if(!type || !init)
    {
        return nullptr;
    }

    emitDebugLocation(node);

    // Codegen
    llvm::Function* func = builder.GetInsertBlock()->getParent();
    auto alloca = createEntryBlockAlloca(func, type->type, node->name->value);

    if(info.emitDebug)
    {
        auto d = dbuilder.createAutoVariable(dblocks.back(), node->name->value,
                                             dfile, node->loc.line, type->dtype,
                                             false);
        dbuilder.insertDeclare(
            alloca, d, dbuilder.createExpression(),
            llvm::DebugLoc::get(node->loc.line, 0, dblocks.back()),
            builder.GetInsertBlock());
    }

    builder.CreateStore(init->value, alloca);

    auto var = std::make_unique<Symbol>(
        std::make_unique<TypedValue>(type, alloca), node->name->value);
    symbols.getTop().insert(std::make_pair(node->name->value, std::move(var)));

    return std::make_unique<TypedValue>(type, init->value);
}
std::unique_ptr<TypedValue>
CodegenVisitor::visit(ast::ASTGlobalVariableDefinitionExpression* node)
{
    Type* type;
    std::unique_ptr<TypedValue> init;
    std::tie(type, init) = inferVariableDefType(node->var.get());

    if(!type || !init)
    {
        return nullptr;
    }

    emitDebugLocation(node);

    // Codegen
    bool isConstant = !node->var->isMutable;
    auto linkage = [&]() {
        if(!node->isExport)
        {
            return llvm::GlobalValue::InternalLinkage;
        }
        return llvm::GlobalValue::WeakODRLinkage;
    }();
    auto zeroinit = type->zeroInit();
    llvm::Constant* llvminit = nullptr;

    if(node->var->init->nodeType == ast::ASTNode::EMPTY_EXPR)
    {
        // if(linkage != llvm::GlobalValue::LinkOnceODRLinkage)
        //{
        return codegenError(node->var.get(),
                            "Global variable has to be initialized");
        //}
        // llvminit = llvm::cast<llvm::Constant>(zeroinit->value);
    }
    else
    {
        auto castedinit = llvm::dyn_cast<llvm::Constant>(init->value);
        if(!castedinit)
        {
            return codegenError(node->var->init.get(),
                                "Global variable has to be initialized "
                                "with a constant expression");
        }
        llvminit = castedinit;
    }

    llvm::GlobalVariable* gvar =
        new llvm::GlobalVariable(*module, type->type, isConstant, linkage,
                                 nullptr, node->var->name->value);
    gvar->setInitializer(llvminit);

    if(info.emitDebug)
    {
        auto d = dbuilder.createGlobalVariable(
            dfile, node->var->name->value, node->var->name->value, dfile,
            node->loc.line, type->dtype, node->isExport, llvminit);
    }

    auto var = std::make_unique<Symbol>(
        std::make_unique<TypedValue>(type, gvar), node->var->name->value);
    var->isExport = node->isExport;
    symbols.getTop().insert(
        std::make_pair(node->var->name->value, std::move(var)));

    return std::make_unique<TypedValue>(type, init->value);
}
std::unique_ptr<TypedValue>
CodegenVisitor::visit(ast::ASTSubscriptExpression* node)
{
    codegenWarning(node, "Unimplemented CodegenVisitor::visit({})",
                   node->nodeType.get());
    return nullptr;
}
std::unique_ptr<TypedValue>
CodegenVisitor::visit(ast::ASTSubscriptRangedExpression* node)
{
    codegenWarning(node, "Unimplemented CodegenVisitor::visit({})",
                   node->nodeType.get());
    return nullptr;
}
std::unique_ptr<TypedValue>
CodegenVisitor::visit(ast::ASTMemberAccessExpression* node)
{
    codegenWarning(node, "Unimplemented CodegenVisitor::visit({})",
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
        return codegenError(var->type.get(),
                            "Function parameter cannot be of unsized type: {}",
                            type->getName());
    }

    if(types.isDefinedUndecorated(var->name->value) != 0)
    {
        return codegenError(
            var->name.get(),
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
        if(!initExpr->type->isSameOrImplicitlyCastable(var->init.get(), builder,
                                                       initExpr->value, type))
        {
            return codegenError(
                var->init.get(),
                "Invalid init expression: Cannot assign {} to {}", typeName,
                type->getDecoratedName());
        }
        initVal = initExpr->value;
    }

    auto alloca = createEntryBlockAlloca(func, llvmtype, var->name->value);

    if(info.emitDebug)
    {
        auto proto =
            static_cast<ast::ASTFunctionPrototypeStatement*>(node->parent);
        auto d = dbuilder.createParameterVariable(
            func->getSubprogram(), var->name->value, node->num, dfile,
            proto->loc.line, type->dtype, true);
        dbuilder.insertDeclare(
            alloca, d, dbuilder.createExpression(),
            llvm::DebugLoc::get(proto->loc.line, 0, func->getSubprogram()),
            builder.GetInsertBlock());
    }

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
CodegenVisitor::visit(ast::ASTFunctionPrototypeStatement* node)
{
    std::vector<llvm::Type*> args;
    args.reserve(node->params.size());
    for(const auto& arg : node->params)
    {
        auto t = types.findDecorated(arg->var->type->value);
        if(!t)
        {
            return nullptr;
        }
        args.push_back(t->type);
    }

    auto rt = types.findDecorated(node->returnType->value);
    if(!rt)
    {
        return nullptr;
    }
    auto retType = rt->type;
    llvm::FunctionType* ft = llvm::FunctionType::get(retType, args, false);

    auto linkage = [&]() {
        auto parent =
            static_cast<ast::ASTFunctionDefinitionStatement*>(node->parent);
        assert(parent);
        if(parent->isDecl)
        {
            return llvm::Function::ExternalLinkage;
        }
        if(node->isExport || node->isMain)
        {
            return llvm::Function::ExternalLinkage;
        }
        return llvm::Function::InternalLinkage;
    }();
    // auto linkage = node->isExport ? llvm::Function::ExternalLinkage
    //                              : llvm::Function::InternalLinkage;
    // auto linkage = llvm::Function::ExternalLinkage;
    llvm::Function* f =
        llvm::Function::Create(ft, linkage, node->name->value, module);

    size_t i = 0;
    for(auto& arg : f->args())
    {
        arg.setName(node->params[i]->var->name->value);
    }

    return createVoidVal(f);
}
std::unique_ptr<TypedValue>
CodegenVisitor::visit(ast::ASTFunctionDefinitionStatement* node)
{
    // Check for function type
    // If absent create one
    const auto proto = node->proto.get();
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

    // Check if it's a declaration
    if(node->isDecl)
    {
        return std::make_unique<TypedValue>(functionType, llvmfunc);
    }
    auto entry = llvm::BasicBlock::Create(context, "entry", llvmfunc);
    builder.SetInsertPoint(entry);

    if(info.emitDebug)
    {
        auto isInternal =
            llvmfunc->getLinkage() == llvm::Function::InternalLinkage;
        auto isDefinition = !node->isDecl;
        auto sp = dbuilder.createFunction(
            dfile, name, llvm::StringRef(), dfile, proto->loc.line,
            static_cast<llvm::DISubroutineType*>(functionType->dtype),
            isInternal, isDefinition, proto->loc.line,
            llvm::DINode::FlagPrototyped, info.optEnabled());
        llvmfunc->setSubprogram(sp);

        dblocks.push_back(sp);
        emitDebugLocation(nullptr);
    }

    symbols.addBlock();

    {
        auto llvmargs = llvmfunc->args();
        assert(node->proto->params.size() == llvmfunc->arg_size());
        auto argit = node->proto->params.begin();
        for(auto llvmargit = llvmargs.begin();
            argit != node->proto->params.end() && llvmargit != llvmargs.end();
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

    emitDebugLocation(node->body.get());
    if(!node->body->accept(this))
    {
        llvmfunc->eraseFromParent();
        symbols.removeTopBlock();
        if(info.emitDebug)
        {
            dblocks.pop_back();
        }
        return nullptr;
    }

    auto backNodeType = [&]() -> ast::ASTNode::NodeType {
        auto& nodes = node->body->nodes;
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
        symbols.removeTopBlock();
        if(info.emitDebug)
        {
            dblocks.pop_back();
        }
        return nullptr;
    }
#endif

    symbols.removeTopBlock();
    if(info.emitDebug)
    {
        dblocks.pop_back();
    }
    return std::make_unique<TypedValue>(functionType, llvmfunc);
}
std::unique_ptr<TypedValue> CodegenVisitor::visit(ast::ASTReturnStatement* node)
{
    emitDebugLocation(node);

    if(node->returnValue->nodeType == ast::ASTNode::EMPTY_EXPR)
    {
        builder.CreateRetVoid();
        return getTypedDummyValue();
    }

    auto ret = node->returnValue->accept(this);
    if(!ret)
    {
        return nullptr;
    }

    auto f = getASTNodeFunction(node);
    auto retType = types.findDecorated(f->returnType->value);
    assert(retType);
    if(!ret->type->isSameOrImplicitlyCastable(node->returnValue.get(), builder,
                                              ret->value, retType))
    {
        codegenInfo(node->getFunction()->proto->returnType.get(),
                    "Function return type defined here");
        return nullptr;
    }

    builder.CreateRet(ret->value);
    return ret;
}

std::unique_ptr<TypedValue>
CodegenVisitor::visit(ast::ASTIntegerLiteralExpression* node)
{
    emitDebugLocation(node);

    auto t = types.findDecorated(node->type->value);
    auto val = [&t, &node]() {
        if(node->isSigned)
        {
            return llvm::ConstantInt::getSigned(t->type, node->value);
        }
        return llvm::ConstantInt::get(
            t->type, static_cast<uint64_t>(node->value), false);
    }();
    return std::make_unique<TypedValue>(t, val);
}
std::unique_ptr<TypedValue>
CodegenVisitor::visit(ast::ASTFloatLiteralExpression* node)
{
    emitDebugLocation(node);

    auto t = types.findDecorated(node->type->value);
    auto val = llvm::ConstantFP::get(t->type, node->value);
    return std::make_unique<TypedValue>(t, val);
}
std::unique_ptr<TypedValue>
CodegenVisitor::visit(ast::ASTStringLiteralExpression* node)
{
    codegenWarning(node, "Unimplemented CodegenVisitor::visit({})",
                   node->nodeType.get());
    return nullptr;
}
std::unique_ptr<TypedValue>
CodegenVisitor::visit(ast::ASTCharLiteralExpression* node)
{
    emitDebugLocation(node);

    auto t = types.findDecorated(node->type->value);
    auto val = llvm::ConstantInt::get(t->type, node->value, false);
    return std::make_unique<TypedValue>(t, val);
}
std::unique_ptr<TypedValue>
CodegenVisitor::visit(ast::ASTBoolLiteralExpression* node)
{
    emitDebugLocation(node);

    auto t = types.findDecorated("bool");
    auto val = node->value ? llvm::ConstantInt::getTrue(context)
                           : llvm::ConstantInt::getFalse(context);
    return std::make_unique<TypedValue>(t, val);
}
std::unique_ptr<TypedValue>
CodegenVisitor::visit(ast::ASTNoneLiteralExpression* node)
{
    codegenWarning(node, "Unimplemented CodegenVisitor::visit({})",
                   node->nodeType.get());
    return nullptr;
}

std::unique_ptr<TypedValue>
CodegenVisitor::visit(ast::ASTBinaryExpression* node)
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

    std::vector<TypedValue*> operands;
    operands.push_back(lhs.get());
    operands.push_back(rhs.get());
    return operands.front()->type->getOperations()->binaryOperation(
        node, builder, node->oper, std::move(operands));
}
std::unique_ptr<TypedValue> CodegenVisitor::visit(ast::ASTUnaryExpression* node)
{

    auto operand = node->operand->accept(this);
    if(!operand)
    {
        return nullptr;
    }

    std::vector<TypedValue*> operands;
    operands.push_back(operand.get());
    return operands.front()->type->getOperations()->unaryOperation(
        node, builder, node->oper, std::move(operands));
}
std::unique_ptr<TypedValue>
CodegenVisitor::visit(ast::ASTAssignmentExpression* node)
{
    emitDebugLocation(node);

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

    std::vector<TypedValue*> operands;
    operands.push_back(lhs.get());
    operands.push_back(rhs.get());
    return operands.front()->type->getOperations()->assignmentOperation(
        node, builder, node->oper, std::move(operands));
}
std::unique_ptr<TypedValue>
CodegenVisitor::visit(ast::ASTArbitraryOperandExpression* node)
{
    emitDebugLocation(node);

    std::vector<std::unique_ptr<TypedValue>> operandsOwned;

    for(auto& o : node->operands)
    {
        auto v = o->accept(this);
        if(!v)
        {
            return nullptr;
        }
        operandsOwned.push_back(std::move(v));
    }

    std::vector<TypedValue*> operands;
    operands.reserve(operandsOwned.size());

    for(auto& o : operandsOwned)
    {
        operands.push_back(o.get());
    }

    return operands.front()->type->getOperations()->arbitraryOperation(
        node, builder, node->oper, std::move(operands));
}

std::unique_ptr<TypedValue> CodegenVisitor::visit(ast::ASTEmptyStatement*)
{
    return getTypedDummyValue();
}
std::unique_ptr<TypedValue> CodegenVisitor::visit(ast::ASTBlockStatement* node)
{
    emitDebugLocation(node);

    symbols.addBlock();

    if(info.emitDebug)
    {
        auto d = dbuilder.createLexicalBlock(dblocks.back(), dfile,
                                             node->loc.line, 0);
        dblocks.push_back(d);
    }

    for(auto& child : node->nodes)
    {
        if(!child->accept(this))
        {
            // Not necessary,
            // execution will be stopped
            // symbols.removeTopBlock();

            return nullptr;
        }
    }

    if(info.emitDebug)
    {
        dblocks.pop_back();
    }
    symbols.removeTopBlock();
    return getTypedDummyValue();
}
std::unique_ptr<TypedValue>
CodegenVisitor::visit(ast::ASTWrappedExpressionStatement* node)
{
    return node->expr->accept(this);
}
} // namespace codegen
