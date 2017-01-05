/*
Copyright (C) 2016-2017 Elias Kosunen

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include "core/ast/AST.h"
#include "core/ast/ASTFunctionStatement.h"
#include "core/ast/ASTNode.h"
#include "core/ast/FwdDecl.h"
#include "core/ast/Visitor.h"
#include "core/codegen/CodegenInfo.h"
#include "core/codegen/Type.h"
#include "util/Logger.h"
#include <llvm/IR/DIBuilder.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/Verifier.h>
#include <utility>

#define USE_LLVM_FUNCTION_VERIFY 0
#define USE_LLVM_MODULE_VERIFY 0

namespace core
{
namespace codegen
{
    class TypedValue
    {
    public:
        Type* type;
        llvm::Value* value;

        TypedValue(Type* t, llvm::Value* v) : type(t), value(v)
        {
        }
    };

    struct Variable
    {
        llvm::Value* value;
        Type* type;
        const std::string& name;
    };

    class CodegenVisitor : public ast::Visitor
    {
        llvm::LLVMContext& context;
        llvm::Module* module;
        const CodegenInfo& info;
        llvm::IRBuilder<> builder;

        llvm::DIBuilder dbuilder;
        llvm::DICompileUnit* dcu;
        std::vector<llvm::DIScope*> dBlocks;

        std::unordered_map<std::string, Variable> variables;
        std::unordered_map<std::string, Variable> globalVariables;
        std::unordered_map<std::string, ast::ASTFunctionPrototypeStatement*>
            functionProtos;
        std::unordered_map<std::string, std::unique_ptr<Type>> types;

        std::array<std::unique_ptr<Type>, 13> _buildTypeArray();
        std::unordered_map<std::string, std::unique_ptr<Type>> _createTypeMap();

        void emitDebugLocation(ast::ASTNode* node);

        std::unique_ptr<TypedValue>
        createVoidVal(llvm::Value* v = nullptr) const
        {
            return std::make_unique<TypedValue>(findType("void"), v);
        }

        llvm::Value* getDummyValue() const
        {
            static llvm::Value* ret =
                llvm::Constant::getNullValue(findType("int32")->type);
            return ret;
        }

        std::unique_ptr<TypedValue> getTypedDummyValue() const
        {
            auto v = getDummyValue();
            return std::make_unique<TypedValue>(findType("int32"), v);
        }

        template <typename... Args>
        std::nullptr_t codegenError(const std::string& format,
                                    Args... args) const
        {
            util::logger->error(format.c_str(), args...);
            return nullptr;
        }

        template <typename... Args>
        void codegenWarning(const std::string& format, Args... args) const
        {
            util::logger->warn(format.c_str(), args...);
        }

        Type* findType(const std::string& name, bool logError = true) const
        {
            auto type = types.find(name);
            if(type == types.end())
            {
                return logError ? codegenError("Undefined typename: '{}'", name)
                                : nullptr;
            }
            return type->second.get();
        }

        Type* findType(llvm::Type* type, bool logError = true) const
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

        llvm::Function* findFunction(const std::string& name)
        {
            llvm::Function* f = module->getFunction(name);
            if(f)
            {
                return f;
            }

            auto fit = functionProtos.find(name);
            if(fit != functionProtos.end())
            {
                return llvm::cast<llvm::Function>(
                    fit->second->accept(this)->value);
            }

            return codegenError("Undefined function: '{}'", name);
        }

        std::string createFunctionName(llvm::Value*)
        {
            return {};
        }

        std::unique_ptr<TypedValue>
        checkTypedValue(std::unique_ptr<TypedValue> val,
                        const Type& requiredType) const
        {
            auto cast = val->type->cast(Type::IMPLICIT, requiredType);
            if(std::get<0>(cast) || std::get<1>(cast))
            {
                return nullptr;
            }
            return val;
        }

        void stripInstructionsAfterTerminators();

        llvm::AllocaInst* createEntryBlockAlloca(llvm::Function* func,
                                                 llvm::Type* type,
                                                 const std::string& name)
        {
            llvm::IRBuilder<> tmp(&func->getEntryBlock(),
                                  func->getEntryBlock().begin());
            return tmp.CreateAlloca(type, nullptr, name.c_str());
        }

        ast::ASTFunctionPrototypeStatement*
        getASTNodeFunction(ast::ASTNode* node) const
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

        llvm::Constant* createStringConstant(const char* str)
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

    public:
        CodegenVisitor(llvm::LLVMContext& c, llvm::Module* m,
                       const CodegenInfo& i);

        bool codegen(ast::AST* ast);

        void dumpModule() const
        {
            module->dump();
        }

        std::unique_ptr<TypedValue> visit(ast::ASTNode* node) = delete;
        std::unique_ptr<TypedValue> visit(ast::ASTStatement* stmt);
        std::unique_ptr<TypedValue> visit(ast::ASTExpression* expr);

        std::unique_ptr<TypedValue> visit(ast::ASTIfStatement* node);
        std::unique_ptr<TypedValue> visit(ast::ASTForStatement* node);
        std::unique_ptr<TypedValue> visit(ast::ASTForeachStatement* node);
        std::unique_ptr<TypedValue> visit(ast::ASTWhileStatement* node);
        std::unique_ptr<TypedValue> visit(ast::ASTImportStatement* node);
        std::unique_ptr<TypedValue> visit(ast::ASTModuleStatement* node);

        std::unique_ptr<TypedValue> visit(ast::ASTEmptyExpression* node);
        std::unique_ptr<TypedValue> visit(ast::ASTIdentifierExpression* node);
        std::unique_ptr<TypedValue> visit(ast::ASTVariableRefExpression* expr);
        std::unique_ptr<TypedValue> visit(ast::ASTCallExpression* expr);
        std::unique_ptr<TypedValue> visit(ast::ASTCastExpression* node);
        std::unique_ptr<TypedValue>
        visit(ast::ASTVariableDefinitionExpression* expr);
        std::unique_ptr<TypedValue> visit(ast::ASTSubscriptExpression* node);
        std::unique_ptr<TypedValue>
        visit(ast::ASTSubscriptRangedExpression* node);
        std::unique_ptr<TypedValue> visit(ast::ASTMemberAccessExpression* node);

        std::unique_ptr<TypedValue> visit(ast::ASTFunctionParameter* node);
        std::unique_ptr<TypedValue>
        visit(ast::ASTFunctionPrototypeStatement* stmt);
        std::unique_ptr<TypedValue>
        visit(ast::ASTFunctionDefinitionStatement* stmt);
        std::unique_ptr<TypedValue>
        visit(ast::ASTFunctionDeclarationStatement* node);
        std::unique_ptr<TypedValue> visit(ast::ASTReturnStatement* stmt);

        std::unique_ptr<TypedValue>
        visit(ast::ASTIntegerLiteralExpression* expr);
        std::unique_ptr<TypedValue> visit(ast::ASTFloatLiteralExpression* expr);
        std::unique_ptr<TypedValue>
        visit(ast::ASTStringLiteralExpression* expr);
        std::unique_ptr<TypedValue> visit(ast::ASTCharLiteralExpression* expr);
        std::unique_ptr<TypedValue> visit(ast::ASTBoolLiteralExpression* expr);
        std::unique_ptr<TypedValue> visit(ast::ASTNoneLiteralExpression* node);

        std::unique_ptr<TypedValue>
        visit(ast::ASTBinaryOperationExpression* expr);
        std::unique_ptr<TypedValue>
        visit(ast::ASTUnaryOperationExpression* expr);
        std::unique_ptr<TypedValue>
        visit(ast::ASTAssignmentOperationExpression* node);

        std::unique_ptr<TypedValue> visit(ast::ASTEmptyStatement* node);
        std::unique_ptr<TypedValue> visit(ast::ASTBlockStatement* stmt);
        std::unique_ptr<TypedValue>
        visit(ast::ASTWrappedExpressionStatement* stmt);
    };
} // namespace codegen
} // namespace core
