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
#include "core/codegen/TypedValue.h"
#include "util/Logger.h"
#include <llvm/IR/DIBuilder.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/Verifier.h>
#include <utility>

namespace core
{
namespace codegen
{
    struct Variable
    {
        Variable(std::unique_ptr<TypedValue> pValue, std::string pName)
            : value(std::move(pValue)), name(std::move(pName))
        {
        }
        Variable(Type* t, llvm::Value* v, std::string pName)
            : value(std::make_unique<TypedValue>(t, v)), name(std::move(pName))
        {
        }

        Type* getType() const
        {
            assert(value && "No value given for Variable");
            return value->type;
        }

        std::unique_ptr<TypedValue> value;
        const std::string name;
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
        class SymbolTable
        {
        public:
            SymbolTable() = default;

            Variable* findSymbol(const std::string& name, Type::Kind type,
                                 bool logError = true);
            Variable* findSymbol(const std::string& name, Type* type = nullptr,
                                 bool logError = true);
            const Variable* findSymbol(const std::string& name, Type::Kind type,
                                       bool logError = true) const;
            const Variable* findSymbol(const std::string& name,
                                       Type* type = nullptr,
                                       bool logError = true) const;

            bool isDefined(const std::string& name, Type::Kind kind) const;
            bool isDefined(const std::string& name, Type* type = nullptr) const;

            void addBlock();
            void removeTopBlock();
            void clear();

            auto& getTop()
            {
                assert(!list.empty() &&
                       "Cannot get the top of a empty symbol list");
                return list.back();
            }
            const auto& getTop() const
            {
                assert(!list.empty() &&
                       "Cannot get the top of a empty symbol list");
                return list.back();
            }

            auto& getList()
            {
                return list;
            }
            const auto& getList() const
            {
                return list;
            }

        private:
            std::vector<
                std::unordered_map<std::string, std::unique_ptr<Variable>>>
                list;
        };

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
        Variable* findFunction(const std::string& name, bool logError = true);
        ast::ASTFunctionPrototypeStatement*
        getASTNodeFunction(ast::ASTNode* node) const;
        Variable* declareFunction(FunctionType* type, const std::string& name);

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

        SymbolTable symbols;
        std::unordered_map<std::string, std::unique_ptr<Type>> types;

    public:
        std::unique_ptr<TypedValue> visit(ast::ASTNode* node) = delete;
        std::unique_ptr<TypedValue> visit(ast::ASTStatement* node);
        std::unique_ptr<TypedValue> visit(ast::ASTExpression* node);

        std::unique_ptr<TypedValue> visit(ast::ASTIfStatement* stmt);
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
        visit(ast::ASTFunctionDeclarationStatement* stmt);
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

    inline std::unique_ptr<TypedValue>
    CodegenVisitor::checkTypedValue(std::unique_ptr<TypedValue> val,
                                    const Type& requiredType) const
    {
        /*auto cast = val->type->cast(Type::IMPLICIT, requiredType);
        if(std::get<0>(cast) || std::get<1>(cast))
        {
            return nullptr;
        }
        return val;*/
        return *val->type == requiredType ? std::move(val) : nullptr;
    }

    inline bool CodegenVisitor::SymbolTable::isDefined(const std::string& name,
                                                       Type::Kind kind) const
    {
        return findSymbol(name, kind);
    }
    inline bool CodegenVisitor::SymbolTable::isDefined(const std::string& name,
                                                       Type* type) const
    {
        return findSymbol(name, type);
    }

    inline void CodegenVisitor::SymbolTable::addBlock()
    {
        list.push_back({});
    }
    inline void CodegenVisitor::SymbolTable::removeTopBlock()
    {
        getTop().clear();
        list.pop_back();
    }

    inline void CodegenVisitor::SymbolTable::clear()
    {
        list.clear();
    }
} // namespace codegen
} // namespace core
