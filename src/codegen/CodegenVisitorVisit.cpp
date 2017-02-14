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

    // Add a new scope
    symbols->addBlock();
    if(info.emitDebug)
    {
        auto d = dbuilder.createLexicalBlock(getTopDebugScope(), dfile,
                                             node->loc.line, 0);
        dblocks.push_back(d);
    }

    // Condition
    auto cond = node->condition->accept(this);
    if(!cond)
    {
        return nullptr;
    }

    auto boolt = types->find("bool");
    assert(boolt);
    // Condition has to be implicitly castable to bool
    auto boolcond = cond->type->cast(node->condition.get(), builder,
                                     Type::IMPLICIT, cond.get(), boolt);
    if(!boolcond)
    {
        return nullptr;
    }
    auto boolcondllvm = boolcond->value;

    llvm::Function* func = builder.GetInsertBlock()->getParent();

    auto thenBB = llvm::BasicBlock::Create(context, "then", func);
    auto elseBB = llvm::BasicBlock::Create(context, "else");
    auto mergeBB = llvm::BasicBlock::Create(context, "ifcont");

    // Is there an else-block
    bool elseExists = node->elseBlock->nodeType != ast::ASTNode::EMPTY_STMT;
    if(elseExists)
    {
        // If there is one,
        // false expression branches to it
        builder.CreateCondBr(boolcondllvm, thenBB, elseBB);
    }
    else
    {
        // If there isn't,
        // false expression branches to the merge block
        builder.CreateCondBr(boolcondllvm, thenBB, mergeBB);
    }

    // Then-block
    builder.SetInsertPoint(thenBB);

    auto thenV = node->ifBlock->accept(this);
    if(!thenV)
    {
        return nullptr;
    }

    builder.CreateBr(mergeBB);
    thenBB = builder.GetInsertBlock();

    // Else-block
    // Only generate if it exists
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

    func->getBasicBlockList().push_back(mergeBB);
    builder.SetInsertPoint(mergeBB);

    // Pop scope
    if(info.emitDebug)
    {
        dblocks.pop_back();
    }
    symbols->removeTopBlock();

    return createVoidVal(mergeBB);
}
std::unique_ptr<TypedValue> CodegenVisitor::visit(ast::ASTForStatement* node)
{
    llvm::Function* func = builder.GetInsertBlock()->getParent();

    emitDebugLocation(node);

    // Push a new scope
    symbols->addBlock();
    if(info.emitDebug)
    {
        auto d = dbuilder.createLexicalBlock(getTopDebugScope(), dfile,
                                             node->loc.line, 0);
        dblocks.push_back(d);
    }

    // init
    //   |
    //   v
    // condition --(false)--
    //   |    ^            |
    // (true) |            |
    //   |    |            |
    //   v    |            |
    // body   |            |
    //   |    |            |
    //   v    |            |
    // step ---            |
    //                     |
    // merge <--------------

    // Loop init expression:
    // for INIT,,
    auto loopInitBB = llvm::BasicBlock::Create(context, "loopinit", func);
    builder.CreateBr(loopInitBB);
    builder.SetInsertPoint(loopInitBB);

    auto init = node->init->accept(this);
    if(!init)
    {
        return nullptr;
    }

    // Loop condition:
    // for ,COND,
    auto loopCondBB = llvm::BasicBlock::Create(context, "loopcond", func);
    builder.SetInsertPoint(loopCondBB);

    auto cond = node->end->accept(this);
    if(!cond)
    {
        return nullptr;
    }

    auto boolt = types->find("bool");
    assert(boolt);
    // Condition has to be implicitly castable to bool
    auto boolcond = cond->type->cast(node->end.get(), builder, Type::IMPLICIT,
                                     cond.get(), boolt);
    if(!boolcond)
    {
        return nullptr;
    }

    // Loop body
    auto loopBodyBB = llvm::BasicBlock::Create(context, "loopbody", func);
    builder.SetInsertPoint(loopBodyBB);

    auto body = node->block->accept(this);
    if(!body)
    {
        return nullptr;
    }

    // Save insert point
    auto bodyInsertBlock = builder.GetInsertBlock();
    auto bodyInsertPoint = builder.GetInsertPoint();

    // Step expression:
    // for ,,STEP
    auto loopStepBB = llvm::BasicBlock::Create(context, "loopstep", func);
    builder.SetInsertPoint(loopStepBB);

    auto step = node->step->accept(this);
    if(!step)
    {
        return nullptr;
    }

    // Loop merge
    auto loopEndBB = llvm::BasicBlock::Create(context, "loopend", func);

    // Loop init always branches to condition
    builder.SetInsertPoint(loopInitBB);
    builder.CreateBr(loopCondBB);

    // Condition has a conditional branch:
    // true: branch to body
    // false: branch to merge/end
    builder.SetInsertPoint(loopCondBB);
    builder.CreateCondBr(boolcond->value, loopBodyBB, loopEndBB);

    // Body branches to step
    builder.SetInsertPoint(bodyInsertBlock, bodyInsertPoint);
    builder.CreateBr(loopStepBB);

    // Step branches to condition
    builder.SetInsertPoint(loopStepBB);
    builder.CreateBr(loopCondBB);

    builder.SetInsertPoint(loopEndBB);

    // Pop scope
    if(info.emitDebug)
    {
        dblocks.pop_back();
    }
    symbols->removeTopBlock();

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

    // Push a new scope
    emitDebugLocation(node);
    symbols->addBlock();

    if(info.emitDebug)
    {
        auto d = dbuilder.createLexicalBlock(getTopDebugScope(), dfile,
                                             node->loc.line, 0);
        dblocks.push_back(d);
    }

    // Condition
    auto condBB = llvm::BasicBlock::Create(context, "loopcond", func);
    builder.CreateBr(condBB);
    builder.SetInsertPoint(condBB);

    auto cond = node->condition->accept(this);
    if(!cond)
    {
        return nullptr;
    }

    auto boolt = types->find("bool");
    assert(boolt);
    // Condition has to be implicitly castable to bool
    auto boolcond = cond->type->cast(node->condition.get(), builder,
                                     Type::IMPLICIT, cond.get(), boolt);
    if(!boolcond)
    {
        return nullptr;
    }

    // Body
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

    // Condition has a conditional branch:
    // true: branch to body
    // false: branch to merge/end
    builder.SetInsertPoint(condBB);
    builder.CreateCondBr(boolcond->value, bodyBB, endBB);

    // Body branches to condition
    builder.SetInsertPoint(bodyInsertBlock, bodyInsertPoint);
    builder.CreateBr(condBB);

    builder.SetInsertPoint(endBB);

    // Pop scope
    if(info.emitDebug)
    {
        dblocks.pop_back();
    }
    symbols->removeTopBlock();

    return getTypedDummyValue();
}
std::unique_ptr<TypedValue> CodegenVisitor::visit(ast::ASTImportStatement* node)
{
    auto table = importModule(node);
    if(!table)
    {
        return nullptr;
    }
    return getTypedDummyValue();
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
    emitDebugLocation(node);

    // Find symbol
    auto symbol = symbols->find(node->value);
    if(!symbol)
    {
        return codegenError(node, "Undefined symbol: '{}'", node->value);
    }
    return std::make_unique<TypedValue>(symbol->getType(), symbol->value->value,
                                        TypedValue::LVALUE, symbol->isMutable);
}
std::unique_ptr<TypedValue>
CodegenVisitor::visit(ast::ASTVariableRefExpression* node)
{
    emitDebugLocation(node);

    // Find symbol
    auto var = symbols->find(node->value);
    if(!var)
    {
        return codegenError(node, "Undefined variable: '{}'", node->value);
    }

    // Create load instruction
    auto load = builder.CreateLoad(var->getType()->type, var->value->value,
                                   node->value.c_str());
    assert(load);
    return std::make_unique<TypedValue>(var->getType(), load,
                                        TypedValue::LVALUE, var->isMutable);
}
std::unique_ptr<TypedValue>
CodegenVisitor::visit(ast::ASTVariableDefinitionExpression* node)
{
    // Infer variable type and codegen init expression
    Type* type;
    std::unique_ptr<TypedValue> init;
    std::tie(type, init) = inferVariableDefType(node);
    if(!type || !init)
    {
        return nullptr;
    }

    // Create alloca instruction
    auto func = builder.GetInsertBlock()->getParent();
    auto alloca = createEntryBlockAlloca(func, type->type, node->name->value);

    emitDebugLocation(node);

    if(info.emitDebug)
    {
        auto d = dbuilder.createAutoVariable(
            getTopDebugScope(), node->name->value, dfile, node->loc.line,
            type->dtype, false);
        dbuilder.insertDeclare(alloca, d, dbuilder.createExpression(),
                               llvm::DebugLoc::get(node->loc.line,
                                                   node->loc.col,
                                                   getTopDebugScope()),
                               builder.GetInsertBlock());
    }

    // Store the initializer value
    builder.CreateStore(init->value, alloca);

    // Define symbol
    auto val = std::make_unique<TypedValue>(type, alloca, TypedValue::LVALUE,
                                            node->isMutable);
    auto valclone = val->clone();
    auto var = std::make_unique<Symbol>(node->loc, std::move(val),
                                        node->name->value, node->isMutable);
    symbols->getTop().insert(std::make_pair(node->name->value, std::move(var)));

    // Add LLVM invariant_start intrinsic
    // This marks the variable as immutable for better optimizations
    if(!node->isMutable)
    {
#if 0
        auto intr = llvm::Intrinsic::getDeclaration(
            module, llvm::Intrinsic::invariant_start,
            {llvm::Type::getInt64Ty(context),
             llvm::Type::getInt8PtrTy(context)});

        auto dataLayout = llvm::DataLayout(module);
        auto size = dataLayout.getTypeSizeInBits(type->type);

        auto call = llvm::CallInst::Create(
            intr,
            {llvm::ConstantInt::get(llvm::Type::getInt64Ty(context), size),
             builder.CreateLoad(alloca)});
        builder.Insert(call);
#endif
    }

    return valclone;
}
std::unique_ptr<TypedValue>
CodegenVisitor::visit(ast::ASTGlobalVariableDefinitionExpression* node)
{
    // Infer variable type and codegen init expression
    Type* type;
    std::unique_ptr<TypedValue> init;
    std::tie(type, init) = inferVariableDefType(node->var.get());
    if(!type || !init)
    {
        return nullptr;
    }

    emitDebugLocation(node);

    // Check initializer
    llvm::Constant* llvminit = nullptr;
    if(node->var->init->nodeType == ast::ASTNode::EMPTY_EXPR)
    {
        return codegenError(node->var.get(),
                            "Global variable has to be initialized");
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

    bool isConstant = !node->var->isMutable;
    // Determine linkage
    auto linkage = [&]() {
        if(!node->isExport)
        {
            return llvm::GlobalValue::InternalLinkage;
        }
        return llvm::GlobalValue::WeakODRLinkage;
    }();

    // Create it
    llvm::GlobalVariable* gvar =
        new llvm::GlobalVariable(*module, type->type, isConstant, linkage,
                                 nullptr, node->var->name->value);
    gvar->setInitializer(llvminit);

    if(info.emitDebug)
    {
        // TODO: Add the debug info to the module
        auto d = dbuilder.createGlobalVariable(
            dfile, node->var->name->value, node->var->name->value, dfile,
            node->loc.line, type->dtype, node->isExport, llvminit);
    }

    // Create symbol
    auto val = std::make_unique<TypedValue>(type, gvar, TypedValue::LVALUE,
                                            node->var->isMutable);
    auto valclone = val->clone();
    auto var =
        std::make_unique<Symbol>(node->loc, std::move(val),
                                 node->var->name->value, node->var->isMutable);
    var->isExport = node->isExport;
    symbols->getTop().insert(
        std::make_pair(node->var->name->value, std::move(var)));

    return valclone;
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

    // Find type
    auto type = types->findDecorated(var->type->value);
    if(!type)
    {
        return codegenError(var->type.get(), "Undefined typename: '{}'",
                            var->type->value);
    }
    llvm::Type* llvmtype = type->type;

    // No 'void' parameters
    if(!type->isSized())
    {
        return codegenError(var->type.get(),
                            "Function parameter cannot be of unsized type: {}",
                            type->getName());
    }

    // Parameter name cannot be a typename
    if(types->isDefinedUndecorated(var->name->value) != 0)
    {
        return codegenError(
            var->name.get(),
            "Cannot name function parameter as '{}': Reserved typename",
            var->name->value);
    }

    // Add alloca-instruction
    auto alloca = createEntryBlockAlloca(func, llvmtype, var->name->value);

    if(info.emitDebug)
    {
        auto proto =
            static_cast<ast::ASTFunctionPrototypeStatement*>(node->parent);
        auto d = dbuilder.createParameterVariable(
            func->getSubprogram(), var->name->value, node->num, dfile,
            proto->loc.line, type->dtype, true);
        dbuilder.insertDeclare(alloca, d, dbuilder.createExpression(),
                               llvm::DebugLoc::get(proto->loc.line,
                                                   proto->loc.col,
                                                   func->getSubprogram()),
                               builder.GetInsertBlock());
    }

    // Declare symbol
    auto val =
        std::make_unique<TypedValue>(type, alloca, TypedValue::LVALUE, false);
    auto valclone = val->clone();
    auto variable = std::make_unique<Symbol>(node->loc, std::move(val),
                                             var->name->value, false);
    symbols->getTop().insert(
        std::make_pair(var->name->value, std::move(variable)));

    return valclone;
}
std::unique_ptr<TypedValue>
CodegenVisitor::visit(ast::ASTFunctionPrototypeStatement* node)
{
    // Find return type
    auto rt = types->findDecorated(node->returnType->value);
    if(!rt)
    {
        return codegenError(node->returnType.get(), "Undefined typename: '{}'",
                            node->returnType->value);
    }
    auto retType = rt->type;

    // Get LLVM types of parameters
    // For creating LLVM function type
    std::vector<llvm::Type*> params;
    params.reserve(node->params.size());
    for(const auto& param : node->params)
    {
        auto t = types->findDecorated(param->var->type->value);
        if(!t)
        {
            return codegenError(param->var->type.get(),
                                "Undefined typename: '{}'",
                                param->var->type->value);
        }
        params.push_back(t->type);
    }

    // Create LLVM function type
    llvm::FunctionType* ft = llvm::FunctionType::get(retType, params, false);

    // Determine linkage
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

    // Create function
    llvm::Function* f =
        llvm::Function::Create(ft, linkage, node->name->value, module);

    // Set argument names
    {
        size_t i = 0;
        for(auto& arg : f->args())
        {
            arg.setName(node->params[i]->var->name->value);
        }
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

    // Find return type
    auto returnType = types->findDecorated(proto->returnType->value);
    if(!returnType)
    {
        return codegenError(proto->returnType.get(), "Undefined typename: '{}'",
                            proto->returnType->value);
    }

    // Find parameter types
    std::vector<Type*> paramTypes;
    for(const auto& p : proto->params)
    {
        auto t = types->findDecorated(p->var->type->value);
        if(!t)
        {
            return codegenError(p->var->type.get(), "Undefined typename: '{}'",
                                p->var->type->value);
        }
        paramTypes.push_back(t);
    }

    // Find function type
    auto functionTypeBase = types->findDecorated(
        FunctionType::functionTypeToString(returnType, paramTypes));
    if(!functionTypeBase)
    {
        // If none was found, create it
        auto ft = std::make_unique<FunctionType>(types.get(), context, dbuilder,
                                                 returnType, paramTypes);
        functionTypeBase = ft.get();
        types->insertType(std::move(ft));
    }
    auto functionType = static_cast<FunctionType*>(functionTypeBase);

    // Declare the function
    auto func = declareFunction(functionType, name, proto);
    if(!func)
    {
        return nullptr;
    }
    auto llvmfunc = llvm::cast<llvm::Function>(func->value->value);

    // If it's a declaration
    // Don't codegen the body,
    // just declare and exit
    if(node->isDecl)
    {
        return std::make_unique<TypedValue>(functionType, llvmfunc,
                                            TypedValue::STMTVALUE, true);
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

    // Push scope
    symbols->addBlock();

    {
        // Codegen params
        auto llvmargs = llvmfunc->args();
        assert(node->proto->params.size() == llvmfunc->arg_size());
        auto argit = node->proto->params.begin();

        // Iterate over llvmargs and function args simultaneously
        for(auto llvmargit = llvmargs.begin();
            argit != node->proto->params.end() && llvmargit != llvmargs.end();
            ++argit, ++llvmargit)
        {
            auto& arg = *argit;

            // Codegen param
            auto vardef = arg->accept(this);
            if(!vardef)
            {
                return nullptr;
            }

            // Store param value in a local variable
            auto& llvmarg = *llvmargit;
            auto alloca = vardef->value;
            builder.CreateStore(&llvmarg, alloca);
        }
    }

    // Codegen body
    emitDebugLocation(node->body.get());
    if(!node->body->accept(this))
    {
        // On failure, remove function and pop scope
        llvmfunc->eraseFromParent();
        symbols->removeTopBlock();
        if(info.emitDebug)
        {
            dblocks.pop_back();
        }
        return nullptr;
    }

    // If no return instruction was found, create one
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
    // Buggy, don't use
    if(!llvm::verifyFunction(*llvmfunc))
    {
        codegenError("Function verification failed");
        util::logger->trace("Invalid function dump:");
        llvmfunc->dump();
        llvmfunc->eraseFromParent();
        symbols->removeTopBlock();
        if(info.emitDebug)
        {
            dblocks.pop_back();
        }
        return nullptr;
    }
#endif

    // Pop scope
    symbols->removeTopBlock();
    if(info.emitDebug)
    {
        dblocks.pop_back();
    }
    return std::make_unique<TypedValue>(functionType, llvmfunc,
                                        TypedValue::STMTVALUE, false);
}
std::unique_ptr<TypedValue> CodegenVisitor::visit(ast::ASTReturnStatement* node)
{
    emitDebugLocation(node);

    // If empty return expression, return void
    if(node->returnValue->nodeType == ast::ASTNode::EMPTY_EXPR)
    {
        builder.CreateRetVoid();
        return getTypedDummyValue();
    }

    // Codegen return expression
    auto ret = node->returnValue->accept(this);
    if(!ret)
    {
        return nullptr;
    }

    // Find function and check if types match
    auto f = getASTNodeFunction(node);
    auto retType = types->findDecorated(f->returnType->value);
    assert(retType);
    if(!ret->type->isSameOrImplicitlyCastable(node->returnValue.get(), builder,
                                              ret.get(), retType))
    {
        // Types don't match
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

    // Find integer type
    auto t = types->findDecorated(node->type->value);
    assert(t);

    return std::make_unique<TypedValue>(
        t,
        [&t, &node]() {
            if(node->isSigned)
            {
                // Unsigned overflow is defined and
                // ConstantInt::get takes a uint64_t,
                // so we could use it even for signed integers,
                // but I think it's cleaner this way
                return llvm::ConstantInt::getSigned(t->type, node->value);
            }
            return llvm::ConstantInt::get(
                t->type, static_cast<uint64_t>(node->value), false);
        }(),
        TypedValue::RVALUE, true);
}
std::unique_ptr<TypedValue>
CodegenVisitor::visit(ast::ASTFloatLiteralExpression* node)
{
    emitDebugLocation(node);

    auto t = types->findDecorated(node->type->value);
    assert(t);

    return std::make_unique<TypedValue>(
        t, llvm::ConstantFP::get(t->type, node->value), TypedValue::RVALUE,
        true);
}
std::unique_ptr<TypedValue>
CodegenVisitor::visit(ast::ASTStringLiteralExpression* node)
{
    emitDebugLocation(node);

    auto t = types->findDecorated(node->type->value);
    assert(t);

    if(node->type->value == "cstring")
    {
        auto stringConst =
            llvm::ConstantDataArray::getString(context, node->value, true);
        auto stringGlobal = new llvm::GlobalVariable(
            *module, stringConst->getType(), true,
            llvm::GlobalValue::InternalLinkage, stringConst, ".str");
        auto indexConst =
            llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), 0);
        std::vector<llvm::Constant*> indexList = {indexConst, indexConst};
        auto stringPtr = llvm::ConstantExpr::getGetElementPtr(
            stringConst->getType(), stringGlobal, indexList, true);

        return std::make_unique<TypedValue>(t, stringPtr, TypedValue::LVALUE,
                                            false);
    }

    auto stringLen = llvm::ConstantInt::get(llvm::Type::getInt64Ty(context),
                                            node->value.length());

    auto stringConst =
        llvm::ConstantDataArray::getString(context, node->value, false);
    auto stringGlobal = new llvm::GlobalVariable(
        *module, stringConst->getType(), true,
        llvm::GlobalValue::InternalLinkage, stringConst, ".str");
    auto indexConst =
        llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), 0);
    std::vector<llvm::Constant*> indexList = {indexConst, indexConst};
    auto stringPtr = llvm::ConstantExpr::getGetElementPtr(
        stringConst->getType(), stringGlobal, indexList, true);
    // auto stringPtr = builder.CreateGlobalStringPtr(node->value, ".str");

    auto stringFatPtr = llvm::ConstantStruct::get(
        llvm::cast<llvm::StructType>(t->type), stringLen, stringPtr, nullptr);
    assert(stringFatPtr);

    return std::make_unique<TypedValue>(t, stringFatPtr, TypedValue::LVALUE,
                                        false);
}
std::unique_ptr<TypedValue>
CodegenVisitor::visit(ast::ASTCharLiteralExpression* node)
{
    emitDebugLocation(node);

    auto t = types->findDecorated(node->type->value);
    assert(t);

    return std::make_unique<TypedValue>(
        t, llvm::ConstantInt::get(t->type, node->value, false),
        TypedValue::RVALUE, true);
}
std::unique_ptr<TypedValue>
CodegenVisitor::visit(ast::ASTBoolLiteralExpression* node)
{
    emitDebugLocation(node);

    auto t = types->findDecorated("bool");
    assert(t);
    return std::make_unique<TypedValue>(
        t, node->value ? llvm::ConstantInt::getTrue(context)
                       : llvm::ConstantInt::getFalse(context),
        TypedValue::RVALUE, true);
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
    emitDebugLocation(node);

    // Codegen lhs
    auto lhs = node->lhs->accept(this);
    if(!lhs)
    {
        return nullptr;
    }

    // Cast expression
    if(node->oper == util::OPERATORB_AS)
    {
        // Check rhs validity
        auto rhs = [&]() -> ast::ASTIdentifierExpression* {
            if(node->rhs->nodeType == ast::ASTNode::IDENTIFIER_EXPR)
            {
                return static_cast<ast::ASTIdentifierExpression*>(
                    node->rhs.get());
            }
            if(node->rhs->nodeType == ast::ASTNode::VARIABLE_REF_EXPR)
            {
                return static_cast<ast::ASTVariableRefExpression*>(
                    node->rhs.get());
            }
            return codegenError(node->rhs.get(),
                                "Invalid cast expression target type");
        }();
        if(!rhs)
        {
            return nullptr;
        }

        // Find type to be casted in
        auto t = types->findDecorated(rhs->value);
        if(!t)
        {
            return codegenError(node->rhs.get(),
                                "Invalid cast: Undefined typename: '{}'",
                                rhs->value);
        }

        // Perform the cast
        return lhs->type->cast(node, builder, Type::CAST, lhs.get(), t);
    }

    // Codegen rhs
    auto rhs = node->rhs->accept(this);
    if(!rhs)
    {
        return nullptr;
    }

    // Create operand vector
    std::vector<TypedValue*> operands{lhs.get(), rhs.get()};

    return operands.front()->type->getOperations()->binaryOperation(
        node, builder, node->oper, std::move(operands));
}
std::unique_ptr<TypedValue> CodegenVisitor::visit(ast::ASTUnaryExpression* node)
{
    emitDebugLocation(node);

    // Codegen operand
    auto operand = node->operand->accept(this);
    if(!operand)
    {
        return nullptr;
    }

    std::vector<TypedValue*> operands{operand.get()};

    return operands.front()->type->getOperations()->unaryOperation(
        node, builder, node->oper, std::move(operands));
}
std::unique_ptr<TypedValue>
CodegenVisitor::visit(ast::ASTAssignmentExpression* node)
{
    emitDebugLocation(node);

    // Codegen lhs
    auto lhs = node->lhs->accept(this);
    if(!lhs)
    {
        return nullptr;
    }

    // Codegen rhs
    auto rhs = node->rhs->accept(this);
    if(!rhs)
    {
        return nullptr;
    }

    std::vector<TypedValue*> operands{lhs.get(), rhs.get()};

    return operands.front()->type->getOperations()->assignmentOperation(
        node, builder, node->oper, std::move(operands));
}
std::unique_ptr<TypedValue>
CodegenVisitor::visit(ast::ASTArbitraryOperandExpression* node)
{
    emitDebugLocation(node);

    assert(node->operands.size() >= 1);
    if(node->operands[0]->nodeType == ast::ASTNode::IDENTIFIER_EXPR &&
       node->operands.size() == 2)
    {
        auto t = types->find(
            static_cast<ast::ASTIdentifierExpression*>(node->operands[0].get())
                ->value);
        // Constructor syntac
        // e.g. let foo = i16(10);
        //              ---^
        // Currently used as cast
        if(t)
        {
            auto param = node->operands[1]->accept(this);
            if(!param)
            {
                return nullptr;
            }
            return param->type->cast(node, builder, Type::CAST, param.get(), t);
        }
    }

    // Codegen operands
    // Need a vector of owned TypedValues to avoid lifetime issues
    std::vector<std::unique_ptr<TypedValue>> operandsOwned;
    operandsOwned.reserve(node->operands.size());

    for(auto& o : node->operands)
    {
        // Codegen operand
        auto v = o->accept(this);
        if(!v)
        {
            return nullptr;
        }
        operandsOwned.push_back(std::move(v));
    }

    // Copy raw pointers to another vector
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

    // Push scope
    symbols->addBlock();
    if(info.emitDebug)
    {
        auto d = dbuilder.createLexicalBlock(getTopDebugScope(), dfile,
                                             node->loc.line, 0);
        dblocks.push_back(d);
    }

    // Codegen each child
    for(auto& child : node->nodes)
    {
        if(!child->accept(this))
        {
            // Not necessary,
            // execution will be stopped
            // symbols->removeTopBlock();

            return nullptr;
        }
    }

    // Pop scope
    if(info.emitDebug)
    {
        dblocks.pop_back();
    }
    symbols->removeTopBlock();

    return getTypedDummyValue();
}
std::unique_ptr<TypedValue>
CodegenVisitor::visit(ast::ASTWrappedExpressionStatement* node)
{
    return node->expr->accept(this);
}
std::unique_ptr<TypedValue> CodegenVisitor::visit(ast::ASTAliasStatement* node)
{
    auto aliasee = types->find(node->aliasee->value);
    if(!aliasee)
    {
        return codegenError(node->aliasee.get(), "Undefined typename: '{}'",
                            node->aliasee->value);
    }

    if(types->isDefined(node->alias->value))
    {
        return codegenError(node->alias.get(),
                            "Cannot overload alias: Type already exists: '{}'",
                            node->alias->value);
    }

    auto type = types->insertType(std::make_unique<AliasType>(
        types.get(), context, dbuilder, node->alias->value, aliasee));
    if(!type)
    {
        return nullptr;
    }
    auto castedType = static_cast<AliasType*>(type);
    type->dtype = dbuilder.createTypedef(castedType->underlying->dtype,
                                         node->alias->value, dfile,
                                         node->loc.line, getTopDebugScope());

    return getTypedDummyValue();
}
} // namespace codegen
