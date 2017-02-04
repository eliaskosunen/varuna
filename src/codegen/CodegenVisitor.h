// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#pragma once

#include "ast/AST.h"
#include "ast/ASTFunctionStatement.h"
#include "ast/ASTNode.h"
#include "ast/FwdDecl.h"
#include "ast/Visitor.h"
#include "codegen/CodegenInfo.h"
#include "codegen/Symbol.h"
#include "codegen/SymbolTable.h"
#include "codegen/Type.h"
#include "codegen/TypeTable.h"
#include "codegen/TypedValue.h"
#include "util/Logger.h"
#include <llvm/IR/DIBuilder.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/Verifier.h>
#include <utility>

namespace codegen
{
class CodegenVisitor final : public ast::Visitor
{
public:
    CodegenVisitor(llvm::LLVMContext& c, llvm::Module* m, CodegenInfo i);

    /// Visit AST
    bool codegen(ast::AST* ast);

    /// Visit AST and get symbols
    /// For importing modules
    std::vector<std::unique_ptr<Symbol>> codegenImport(ast::AST* ast);

    /// Dump the module to stdout
    void dumpModule() const
    {
        module->dump();
    }

private:
    void emitDebugLocation(ast::ASTNode* node);

    void writeExports();

    /// Create a new void-typed value
    std::unique_ptr<TypedValue> createVoidVal(llvm::Value* v = nullptr);
    /// Get a dummy LLVM value
    llvm::Value* getDummyValue();
    /// Get a typed dummy value
    std::unique_ptr<TypedValue> getTypedDummyValue();

    template <typename... Args>
    std::nullptr_t codegenError(const std::string& format, Args... args) const;
    template <typename... Args>
    void codegenWarning(const std::string& format, Args... args) const;

    /// Find a function by name
    FunctionSymbol* findFunction(const std::string& name, bool logError = true);
    /// Get the prototype of the function of node
    ast::ASTFunctionPrototypeStatement*
    getASTNodeFunction(ast::ASTNode* node) const;
    /// Declare function
    FunctionSymbol* declareFunction(FunctionType* type, const std::string& name,
                                    ast::ASTFunctionPrototypeStatement* proto);

    /// Remove all instructions after block terminators
    /// Also add 'unreachable'-instruction if no terminators are found
    void stripInstructionsAfterTerminators();

    /// Create 'alloca'-instruction for variable
    llvm::AllocaInst* createEntryBlockAlloca(llvm::Function* func,
                                             llvm::Type* type,
                                             const std::string& name);

    llvm::Constant* createStringConstant(const char* str);

    std::tuple<Type*, std::unique_ptr<TypedValue>>
    inferVariableDefType(ast::ASTVariableDefinitionExpression* expr);

    llvm::LLVMContext& context;
    llvm::Module* module;
    CodegenInfo info;
    llvm::IRBuilder<> builder;

    llvm::DIBuilder dbuilder;
    llvm::DICompileUnit* dcu;
    llvm::DIFile* dfile;
    std::vector<llvm::DIScope*> dblocks;

    SymbolTable symbols;
    TypeTable types;

public:
    std::unique_ptr<TypedValue> visit(ast::ASTNode* node) = delete;
    std::unique_ptr<TypedValue> visit(ast::ASTStatement* node);
    std::unique_ptr<TypedValue> visit(ast::ASTExpression* node);

    std::unique_ptr<TypedValue> visit(ast::ASTIfStatement* node);
    std::unique_ptr<TypedValue> visit(ast::ASTForStatement* node);
    std::unique_ptr<TypedValue> visit(ast::ASTForeachStatement* node);
    std::unique_ptr<TypedValue> visit(ast::ASTWhileStatement* node);
    std::unique_ptr<TypedValue> visit(ast::ASTImportStatement* node);
    std::unique_ptr<TypedValue> visit(ast::ASTModuleStatement* node);

    std::unique_ptr<TypedValue> visit(ast::ASTEmptyExpression* node);
    std::unique_ptr<TypedValue> visit(ast::ASTIdentifierExpression* node);
    std::unique_ptr<TypedValue> visit(ast::ASTVariableRefExpression* expr);
    std::unique_ptr<TypedValue> visit(ast::ASTCastExpression* node);
    std::unique_ptr<TypedValue>
    visit(ast::ASTVariableDefinitionExpression* expr);
    std::unique_ptr<TypedValue>
    visit(ast::ASTGlobalVariableDefinitionExpression* expr);
    std::unique_ptr<TypedValue> visit(ast::ASTSubscriptExpression* node);
    std::unique_ptr<TypedValue> visit(ast::ASTSubscriptRangedExpression* node);
    std::unique_ptr<TypedValue> visit(ast::ASTMemberAccessExpression* node);

    std::unique_ptr<TypedValue> visit(ast::ASTFunctionParameter* node);
    std::unique_ptr<TypedValue> visit(ast::ASTFunctionPrototypeStatement* node);
    std::unique_ptr<TypedValue>
    visit(ast::ASTFunctionDefinitionStatement* node);
    std::unique_ptr<TypedValue> visit(ast::ASTReturnStatement* node);

    std::unique_ptr<TypedValue> visit(ast::ASTIntegerLiteralExpression* expr);
    std::unique_ptr<TypedValue> visit(ast::ASTFloatLiteralExpression* expr);
    std::unique_ptr<TypedValue> visit(ast::ASTStringLiteralExpression* expr);
    std::unique_ptr<TypedValue> visit(ast::ASTCharLiteralExpression* expr);
    std::unique_ptr<TypedValue> visit(ast::ASTBoolLiteralExpression* expr);
    std::unique_ptr<TypedValue> visit(ast::ASTNoneLiteralExpression* node);

    std::unique_ptr<TypedValue> visit(ast::ASTBinaryOperationExpression* expr);
    std::unique_ptr<TypedValue> visit(ast::ASTUnaryOperationExpression* expr);
    std::unique_ptr<TypedValue>
    visit(ast::ASTAssignmentOperationExpression* node);
    std::unique_ptr<TypedValue>
    visit(ast::ASTArbitraryOperationExpression* node);

    std::unique_ptr<TypedValue> visit(ast::ASTEmptyStatement* node);
    std::unique_ptr<TypedValue> visit(ast::ASTBlockStatement* node);
    std::unique_ptr<TypedValue> visit(ast::ASTWrappedExpressionStatement* node);
};

inline std::unique_ptr<TypedValue> CodegenVisitor::createVoidVal(llvm::Value* v)
{
    static auto t = types.findDecorated("void");
    assert(t);
    return std::make_unique<TypedValue>(t, v);
}

inline llvm::Value* CodegenVisitor::getDummyValue()
{
    return getTypedDummyValue()->value;
}

inline std::unique_ptr<TypedValue> CodegenVisitor::getTypedDummyValue()
{
    static auto t = types.findDecorated("int32");
    assert(t);
    static auto v = llvm::Constant::getNullValue(t->type);
    auto ret = std::make_unique<TypedValue>(t, v);
    return ret;
}

template <typename... Args>
inline std::nullptr_t CodegenVisitor::codegenError(const std::string& format,
                                                   Args... args) const
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
} // namespace codegen
