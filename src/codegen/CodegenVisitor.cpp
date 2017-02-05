// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#include "codegen/CodegenVisitor.h"
#include "ast/ASTControlStatement.h"
#include "ast/ASTExpression.h"
#include "ast/ASTFunctionStatement.h"
#include "ast/ASTLiteralExpression.h"
#include "ast/ASTNode.h"
#include "ast/ASTOperatorExpression.h"
#include "ast/ASTStatement.h"
#include "ast/FwdDecl.h"
#include "util/ProgramInfo.h"
#include "util/ProgramOptions.h"

#define USE_LLVM_MODULE_VERIFY 0

namespace codegen
{
CodegenVisitor::CodegenVisitor(llvm::LLVMContext& c, llvm::Module* m,
                               CodegenInfo i)
    : context{c}, module(m), info{i}, builder(context), dbuilder(*m),
      dcu{dbuilder.createCompileUnit(
          llvm::dwarf::DW_LANG_C, info.file->getFilename(), ".",
          util::programinfo::getIdentifier(), info.optEnabled(), "", 0)},
      dfile{dbuilder.createFile(dcu->getFilename(), dcu->getDirectory())}
{
    types.insertTypeWithVariants<VoidType>(context, dbuilder, false);
    types.insertTypeWithVariants<IntType>(context, dbuilder, true);
    types.insertTypeWithVariants<Int8Type>(context, dbuilder, true);
    types.insertTypeWithVariants<Int16Type>(context, dbuilder, true);
    types.insertTypeWithVariants<Int32Type>(context, dbuilder, true);
    types.insertTypeWithVariants<Int64Type>(context, dbuilder, true);
    types.insertTypeWithVariants<FloatType>(context, dbuilder, true);
    types.insertTypeWithVariants<F32Type>(context, dbuilder, true);
    types.insertTypeWithVariants<F64Type>(context, dbuilder, true);
    types.insertTypeWithVariants<BoolType>(context, dbuilder, true);
    types.insertTypeWithVariants<CharType>(context, dbuilder, true);
    types.insertTypeWithVariants<ByteCharType>(context, dbuilder, true);
    types.insertTypeWithVariants<ByteType>(context, dbuilder, true);
    types.insertTypeWithVariants<StringType>(context, dbuilder, true);
}

bool CodegenVisitor::codegen(ast::AST* ast)
{
    if(info.emitDebug)
    {
        module->addModuleFlag(llvm::Module::Warning, "Debug Info Version",
                              llvm::DEBUG_METADATA_VERSION);
    }

    module->setSourceFileName(ast->file->getFilename());

    auto root = ast->globalNode.get();
    symbols.addBlock();
    for(auto& child : root->nodes)
    {
        if(!child->accept(this))
        {
            return false;
        }
    }

    auto exports = symbols.findExports();

    symbols.dump();
    symbols.removeTopBlock();

    stripInstructionsAfterTerminators();

    dbuilder.finalize();

#if USE_LLVM_MODULE_VERIFY
    util::logger->trace("Verifying module");
    if(!llvm::verifyModule(*module))
    {
        util::logger->error("Module verification failed");
        util::loggerBasic->info("Module dump:");
        dumpModule();
        return false;
    }
    util::logger->trace("Module verification successful");
#endif

    return true;
}

void CodegenVisitor::emitDebugLocation(ast::ASTNode* node)
{
    if(info.emitDebug)
    {
        if(!node)
        {
            builder.SetCurrentDebugLocation(llvm::DebugLoc());
            return;
        }

        auto scope = [&]() -> llvm::DIScope* {
            if(dblocks.empty())
            {
                return dcu;
            }
            return dblocks.back();
        }();

        builder.SetCurrentDebugLocation(
            llvm::DebugLoc::get(node->loc.line, node->loc.col, scope));
    }
}

void CodegenVisitor::writeExports()
{
    if(util::viewProgramOptions().outputFilename == "stdout")
    {
        util::logger->info("Not writing module export file");
        return;
    }

    auto filename = fmt::format("{}.vamod", info.file->getFilename());
}

void CodegenVisitor::stripInstructionsAfterTerminators()
{
    for(auto& func : *module)
    {
        for(auto& bb : func)
        {
            bool termFound = false;
            for(auto inst = bb.begin(); inst != bb.end();)
            {
                if(!termFound && inst->isTerminator())
                {
                    termFound = true;
                    ++inst;
                    continue;
                }
                if(termFound)
                {
                    inst = inst->eraseFromParent();
                }
                else
                {
                    ++inst;
                }
            }
            if(!termFound)
            {
                builder.SetInsertPoint(&bb);
                builder.CreateUnreachable();
            }
        }
    }
}

FunctionSymbol* CodegenVisitor::findFunction(const std::string& name,
                                             ast::ASTNode* node, bool logError)
{
    auto f = symbols.find(name, Type::FUNCTION, false);
    if(f)
    {
        auto func = static_cast<FunctionSymbol*>(f);
        assert(func->value->value);
        return func;
    }

    if(logError)
    {
        return codegenError(node, "Undefined function: '{}'", name);
    }
    return nullptr;
}

FunctionSymbol*
CodegenVisitor::declareFunction(FunctionType* type, const std::string& name,
                                ast::ASTFunctionPrototypeStatement* proto)
{
    // Check if function with the same name is already declared
    auto func = findFunction(name, nullptr, false);
    if(func)
    {
        // Check for type
        if(func->value->type == type)
        {
            return func;
        }
        else
        {
            return codegenError(proto,
                                "Function declaration failed: Mismatching "
                                "prototypes for similarly named functions: "
                                "'{}' and '{}'",
                                func->value->type->getDecoratedName(),
                                type->getDecoratedName());
        }
    }

    // Codegen prototype
    auto accept = proto->accept(this);
    if(!accept)
    {
        return nullptr;
    }

    // Add symbol to current scope
    auto var =
        std::make_unique<FunctionSymbol>(type, accept->value, name, proto);
    var->isExport = proto->isExport;
    auto varptr = var.get();
    symbols.getTop().insert(std::make_pair(name, std::move(var)));
    return varptr;
}

llvm::AllocaInst*
CodegenVisitor::createEntryBlockAlloca(llvm::Function* func, llvm::Type* type,
                                       const std::string& name)
{
    llvm::IRBuilder<> tmp(&func->getEntryBlock(),
                          func->getEntryBlock().begin());
    return tmp.CreateAlloca(type, nullptr, name.c_str());
}

ast::ASTFunctionPrototypeStatement*
CodegenVisitor::getASTNodeFunction(ast::ASTNode* node) const
{
    auto f = node->getFunction();
    if(!f)
    {
        return nullptr;
    }

    auto casted = dynamic_cast<ast::ASTFunctionDefinitionStatement*>(f);
    return casted->proto.get();
}

llvm::Constant* CodegenVisitor::createStringConstant(const char* str)
{
    llvm::Constant* strConst = llvm::ConstantDataArray::getString(context, str);
    auto gvStr = new llvm::GlobalVariable(*module, strConst->getType(), true,
                                          llvm::GlobalValue::InternalLinkage,
                                          nullptr, ".str");
    gvStr->setAlignment(1);
    gvStr->setInitializer(strConst);

    llvm::Constant* strVal =
        llvm::ConstantExpr::getGetElementPtr(strConst->getType(), gvStr, {});
    return strVal;
}

std::tuple<Type*, std::unique_ptr<TypedValue>>
CodegenVisitor::inferVariableDefType(ast::ASTVariableDefinitionExpression* node)
{
    auto ret = [](Type* t, std::unique_ptr<TypedValue> i) {
        return std::make_tuple(t, std::move(i));
    };
    auto err = [](std::nullptr_t n = nullptr) { return std::make_tuple(n, n); };

    // Initializer
    std::unique_ptr<TypedValue> init = nullptr;
    if(node->init->nodeType != ast::ASTNode::EMPTY_EXPR)
    {
        init = node->init->accept(this);
        if(!init)
        {
            return err();
        }
    }

    // Infer type
    Type* type = nullptr;
    if(node->typeInferred)
    {
        type = types.find(init->type->getName(), node->isMutable
                                                     ? TypeTable::FIND_MUTABLE
                                                     : TypeTable::FIND_DEFAULT);
    }
    else
    {
        type = types.find(node->type->value, node->isMutable
                                                 ? TypeTable::FIND_MUTABLE
                                                 : TypeTable::FIND_DEFAULT);
    }
    if(!type)
    {
        return err();
    }

    // Type checks
    if(!type->isSized())
    {
        return err(codegenError(node->type.get(),
                                "Cannot create a variable of unsized type: {}",
                                type->getDecoratedName()));
    }
    if(types.isDefinedUndecorated(node->name->value) != 0)
    {
        return err(codegenError(
            node->type.get(), "Cannot name variable as '{}': Reserved typename",
            node->name->value));
    }

    // No initializer:
    // Init as undefined
    if(!init)
    {
        if(node->isMutable)
        {
            codegenWarning(node, "Uninitialized variable: {}",
                           node->name->value);
        }
        else
        {
            codegenWarning(
                node, "Useless variable: Uninitialized immutable variable: {}",
                node->name->value);
        }
        init = std::make_unique<TypedValue>(type,
                                            llvm::UndefValue::get(type->type));
        if(!init)
        {
            return err();
        }
    }

    // Check init type
    if(!init->type->isSameOrImplicitlyCastable(node->init.get(), builder,
                                               init->value, type))
    {
        return err(codegenError(
            node->init.get(), "Invalid init nodeession: Cannot assign {} to {}",
            init->type->getDecoratedName(), type->getDecoratedName()));
    }

    // Check for existing similarly named symbol
    if(symbols.find(node->name->value, nullptr, false))
    {
        return err(
            codegenError(node->name.get(),
                         "Cannot name variable as '{}', name already defined",
                         node->name->value));
    }

    return ret(type, std::move(init));
}
} // namespace codegen
