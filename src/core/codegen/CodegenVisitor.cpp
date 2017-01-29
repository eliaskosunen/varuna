// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#include "core/codegen/CodegenVisitor.h"
#include "core/ast/ASTControlStatement.h"
#include "core/ast/ASTExpression.h"
#include "core/ast/ASTFunctionStatement.h"
#include "core/ast/ASTLiteralExpression.h"
#include "core/ast/ASTNode.h"
#include "core/ast/ASTOperatorExpression.h"
#include "core/ast/ASTStatement.h"
#include "core/ast/FwdDecl.h"
#include "util/ProgramInfo.h"
#include "util/ProgramOptions.h"

#define USE_LLVM_MODULE_VERIFY 0

namespace core
{
namespace codegen
{
    CodegenVisitor::CodegenVisitor(llvm::LLVMContext& c, llvm::Module* m,
                                   const CodegenInfo& i)
        : context{c}, module(m), info{i}, builder(context), dbuilder(*m),
          dcu{dbuilder.createCompileUnit(
              llvm::dwarf::DW_LANG_C, info.filename, ".",
              util::programinfo::getIdentifier(), info.optEnabled(), "", 0)}
    {
        types.insertTypeWithVariants<VoidType>(context, dbuilder, false);
        types.insertTypeWithVariants<IntType>(context, dbuilder);
        types.insertTypeWithVariants<Int8Type>(context, dbuilder);
        types.insertTypeWithVariants<Int16Type>(context, dbuilder);
        types.insertTypeWithVariants<Int32Type>(context, dbuilder);
        types.insertTypeWithVariants<Int64Type>(context, dbuilder);
        types.insertTypeWithVariants<FloatType>(context, dbuilder);
        types.insertTypeWithVariants<F32Type>(context, dbuilder);
        types.insertTypeWithVariants<F64Type>(context, dbuilder);
        types.insertTypeWithVariants<BoolType>(context, dbuilder);
        types.insertTypeWithVariants<CharType>(context, dbuilder);
        types.insertTypeWithVariants<ByteCharType>(context, dbuilder);
        types.insertTypeWithVariants<ByteType>(context, dbuilder);
        types.insertTypeWithVariants<StringType>(context, dbuilder);
    }

    bool CodegenVisitor::codegen(ast::AST* ast)
    {
        module->setSourceFileName(ast->file);

        auto root = ast->globalNode.get();
        symbols.addBlock();
        for(auto& child : root->nodes)
        {
            if(!child->accept(this))
            {
                codegenWarning("Block child codegen failed: {}",
                               child->nodeType.get());
                return false;
            }
        }
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

    void CodegenVisitor::writeExports()
    {
        if(util::getProgramOptions().outputFilename == "stdout")
        {
            util::logger->info("Not writing module export file");
            return;
        }

        auto filename = fmt::format("{}.vaexport", info.filename);
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
                                                 bool logError)
    {
        auto f = symbols.find(name, Type::FUNCTION, logError);
        if(f)
        {
            auto func = static_cast<FunctionSymbol*>(f);
            assert(func->value->value);
            return func;
        }

        if(logError)
        {
            return codegenError("Undefined function: '{}'", name);
        }
        return nullptr;
    }

    FunctionSymbol*
    CodegenVisitor::declareFunction(FunctionType* type, const std::string& name,
                                    ast::ASTFunctionPrototypeStatement* proto)
    {
        // Check if function with the same name is already declared
        auto func = findFunction(name, false);
        if(func)
        {
            // Check for type
            if(func->value->type == type)
            {
                return func;
            }
            else
            {
                return codegenError("Function declaration failed: Mismatching "
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
        auto varptr = var.get();
        symbols.getTop().insert(std::make_pair(name, std::move(var)));
        return varptr;
    }

    llvm::AllocaInst* CodegenVisitor::createEntryBlockAlloca(
        llvm::Function* func, llvm::Type* type, const std::string& name)
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
        llvm::Constant* strConst =
            llvm::ConstantDataArray::getString(context, str);
        auto gvStr = new llvm::GlobalVariable(
            *module, strConst->getType(), true,
            llvm::GlobalValue::InternalLinkage, nullptr, ".str");
        gvStr->setAlignment(1);
        gvStr->setInitializer(strConst);

        llvm::Constant* strVal = llvm::ConstantExpr::getGetElementPtr(
            strConst->getType(), gvStr, {});
        return strVal;
    }

    std::tuple<Type*, std::unique_ptr<TypedValue>>
    CodegenVisitor::inferVariableDefType(
        ast::ASTVariableDefinitionExpression* expr)
    {
        auto ret = [](Type* t, std::unique_ptr<TypedValue> i) {
            return std::make_tuple(t, std::move(i));
        };
        auto err = [](std::nullptr_t n = nullptr) {
            return std::make_tuple(n, n);
        };

        // Initializer
        std::unique_ptr<TypedValue> init = nullptr;
        if(expr->init->nodeType != ast::ASTNode::EMPTY_EXPR)
        {
            init = expr->init->accept(this);
            if(!init)
            {
                return err();
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
            return err();
        }

        // Type checks
        if(!type->isSized())
        {
            return err(
                codegenError("Cannot create a variable of unsized type: {}",
                             type->getDecoratedName()));
        }
        if(types.isDefinedUndecorated(expr->name->value) != 0)
        {
            return err(
                codegenError("Cannot name variable as '{}': Reserved typename",
                             expr->name->value));
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
                return err();
            }
        }

        // Check init type
        if(!init->type->isSameOrImplicitlyCastable(builder, init->value, type))
        {
            return err(codegenError(
                "Invalid init expression: Cannot assign {} to {}",
                init->type->getDecoratedName(), type->getDecoratedName()));
        }

        // Check for existing similarly named symbol
        if(symbols.find(expr->name->value, nullptr, false))
        {
            return err(codegenError(
                "Cannot name variable as '{}', name already defined",
                expr->name->value));
        }

        return ret(type, std::move(init));
    }
} // namespace codegen
} // namespace core
