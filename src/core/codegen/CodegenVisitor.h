// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

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
    struct TypedValue
    {
        TypedValue(Type* t, llvm::Value* v) : type(t), value(v)
        {
        }

        Type* type;
        llvm::Value* value;
    };

    struct Variable
    {
        llvm::Value* value;
        Type* type;
        const std::string& name;
    };

    class CodegenVisitor final : public ast::Visitor
    {
    public:
        CodegenVisitor(llvm::LLVMContext& c, llvm::Module* m,
                       const CodegenInfo& i);

        bool codegen(ast::AST* ast);

        void dumpModule() const
        {
            module->dump();
        }

    private:
        void emitDebugLocation(ast::ASTNode* node);

        std::unique_ptr<TypedValue>
        createVoidVal(llvm::Value* v = nullptr) const;
        llvm::Value* getDummyValue() const;
        std::unique_ptr<TypedValue> getTypedDummyValue() const;

        std::unique_ptr<TypedValue>
        checkTypedValue(std::unique_ptr<TypedValue> val,
                        const Type& requiredType) const;
        std::array<std::unique_ptr<Type>, 13> _buildTypeArray();
        std::unordered_map<std::string, std::unique_ptr<Type>> _createTypeMap();

        template <typename... Args>
        std::nullptr_t codegenError(const std::string& format,
                                    Args... args) const;
        template <typename... Args>
        void codegenWarning(const std::string& format, Args... args) const;

        Type* findType(const std::string& name, bool logError = true) const;
        Type* findType(llvm::Type* type, bool logError = true) const;
        llvm::Function* findFunction(const std::string& name);
        ast::ASTFunctionPrototypeStatement*
        getASTNodeFunction(ast::ASTNode* node) const;

        void stripInstructionsAfterTerminators();

        llvm::AllocaInst* createEntryBlockAlloca(llvm::Function* func,
                                                 llvm::Type* type,
                                                 const std::string& name);

        llvm::Constant* createStringConstant(const char* str);

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

    public:
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

    inline std::unique_ptr<TypedValue>
    CodegenVisitor::createVoidVal(llvm::Value* v) const
    {
        return std::make_unique<TypedValue>(findType("void"), v);
    }

    inline llvm::Value* CodegenVisitor::getDummyValue() const
    {
        static llvm::Value* ret =
            llvm::Constant::getNullValue(findType("int32")->type);
        return ret;
    }

    inline std::unique_ptr<TypedValue>
    CodegenVisitor::getTypedDummyValue() const
    {
        auto v = getDummyValue();
        return std::make_unique<TypedValue>(findType("int32"), v);
    }

    template <typename... Args>
    inline std::nullptr_t
    CodegenVisitor::codegenError(const std::string& format, Args... args) const
    {
        util::logger->error(format.c_str(), args...);
        return nullptr;
    }

    template <typename... Args>
    inline void CodegenVisitor::codegenWarning(const std::string& format,
                                               Args... args) const
    {
        util::logger->warn(format.c_str(), args...);
    }

    std::unique_ptr<TypedValue> inline CodegenVisitor::checkTypedValue(
        std::unique_ptr<TypedValue> val, const Type& requiredType) const
    {
        auto cast = val->type->cast(Type::IMPLICIT, requiredType);
        if(std::get<0>(cast) || std::get<1>(cast))
        {
            return nullptr;
        }
        return val;
    }
} // namespace codegen
} // namespace core
