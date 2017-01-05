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

namespace core
{
namespace codegen
{
    Type* CodegenVisitor::findType(const std::string& name, bool logError) const
    {
        auto type = types.find(name);
        if(type == types.end())
        {
            return logError ? codegenError("Undefined typename: '{}'", name)
                            : nullptr;
        }
        return type->second.get();
    }

    Type* CodegenVisitor::findType(llvm::Type* type, bool logError) const
    {
        for(const auto& t : types)
        {
            if(t.second->type == type)
            {
                return t.second.get();
            }
        }
        if(logError)
        {
            codegenError("Undefined type:");
            type->dump();
        }
        return nullptr;
    }

    std::array<std::unique_ptr<Type>, 13> CodegenVisitor::_buildTypeArray()
    {
// Forgive me
#define CODEGEN_TYPE(T) std::make_unique<T>(context, dbuilder)

        return {{
            CODEGEN_TYPE(VoidType), CODEGEN_TYPE(IntType),
            CODEGEN_TYPE(Int8Type), CODEGEN_TYPE(Int16Type),
            CODEGEN_TYPE(Int32Type), CODEGEN_TYPE(Int64Type),
            CODEGEN_TYPE(BoolType), CODEGEN_TYPE(CharType),
            CODEGEN_TYPE(ByteType), CODEGEN_TYPE(FloatType),
            CODEGEN_TYPE(F32Type), CODEGEN_TYPE(F64Type),
            CODEGEN_TYPE(StringType),
        }};

#undef CODEGEN_TYPE
    }

    std::unordered_map<std::string, std::unique_ptr<Type>>
    CodegenVisitor::_createTypeMap()
    {
        auto arr = _buildTypeArray();
        std::unordered_map<std::string, std::unique_ptr<Type>> ret;

        for(auto& i : arr)
        {
            ret.insert(std::make_pair(i->name, std::move(i)));
        }
        return ret;
    }

    CodegenVisitor::CodegenVisitor(llvm::LLVMContext& c, llvm::Module* m,
                                   const CodegenInfo& i)
        : context{c}, module(m), info{i}, builder(context), dbuilder(*m),
          dcu{dbuilder.createCompileUnit(
              llvm::dwarf::DW_LANG_C, info.filename, ".",
              util::programinfo::getIdentifier(), info.optEnabled(), "", 0)},
          dBlocks{}, variables{}, globalVariables{}, functionProtos{},
          types{_createTypeMap()}
    {
    }

    bool CodegenVisitor::codegen(ast::AST* ast)
    {
        module->setSourceFileName(ast->file);

        auto root = ast->globalNode.get();
        for(auto& child : root->nodes)
        {
            if(!child->accept(this))
            {
                util::logger->debug("visit failed, child->nodeType: {}",
                                    child->nodeType);
                return false;
            }
        }

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
            }
        }
    }

    llvm::Function* CodegenVisitor::findFunction(const std::string& name)
    {
        llvm::Function* f = module->getFunction(name);
        if(f)
        {
            return f;
        }

        auto fit = functionProtos.find(name);
        if(fit != functionProtos.end())
        {
            return llvm::cast<llvm::Function>(fit->second->accept(this)->value);
        }

        return codegenError("Undefined function: '{}'", name);
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

        if(f->nodeType == ast::ASTNode::FUNCTION_DECL_STMT)
        {
            auto casted =
                dynamic_cast<ast::ASTFunctionDeclarationStatement*>(f);
            return casted->proto.get();
        }

        auto casted = dynamic_cast<ast::ASTFunctionDefinitionStatement*>(f);
        return casted->proto.get();
    }

    llvm::Constant* CodegenVisitor::createStringConstant(const char* str)
    {
        llvm::Constant* strConst =
            llvm::ConstantDataArray::getString(context, str);
        llvm::GlobalVariable* gvStr = new llvm::GlobalVariable(
            *module, strConst->getType(), true,
            llvm::GlobalValue::InternalLinkage, 0, ".str");
        gvStr->setAlignment(1);
        gvStr->setInitializer(strConst);

        llvm::Constant* strVal = llvm::ConstantExpr::getGetElementPtr(
            strConst->getType(), gvStr, {});
        return strVal;
    }
} // namespace codegen
} // namespace core
