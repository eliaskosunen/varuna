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
/// Visits the AST and generates code for it.
/// The heart of codegen
class CodegenVisitor final : public ast::Visitor
{
public:
    CodegenVisitor(llvm::LLVMContext& c, llvm::Module* m, CodegenInfo i);

    /**
     * Visit the AST and generate code for it
     * @param  ast AST to visit
     * @return     Success
     */
    bool codegen(ast::AST* ast);

    /// Dump the module to stdout
    void dumpModule() const
    {
        module->dump();
    }

private:
    /**
     * Add current source location information to the module.
     * Checks for '-g' itself.
     * @param node Location to use, or nullptr to reset location information
     * (e.g. for function prologues)
     */
    void emitDebugLocation(ast::ASTNode* node);

    std::string getModuleFilename() const;
    std::string getModuleFilename(const std::string& moduleName) const;

    void writeExports(std::unique_ptr<SymbolTable> exports);

    /// Create a new void-typed value
    std::unique_ptr<TypedValue> createVoidVal(llvm::Value* v = nullptr);
    /// Get a dummy LLVM value
    llvm::Value* getDummyValue();
    /// Get a typed dummy value
    std::unique_ptr<TypedValue> getTypedDummyValue();

    bool importModule(ast::ASTImportStatement* import);

    /**
     * Log an error
     * @param  node   Errorenous node
     * @param  format Message format
     * @param  args   Format arguments
     * @return        nullptr
     */
    template <typename... Args>
    std::nullptr_t codegenError(ast::ASTNode* node, const std::string& format,
                                Args&&... args) const;
    template <typename... Args>
    void codegenWarning(ast::ASTNode* node, const std::string& format,
                        Args&&... args) const;
    template <typename... Args>
    void codegenInfo(ast::ASTNode* node, const std::string& format,
                     Args&&... args) const;

    /**
     * Find a function by name
     * @param  name     Function name
     * @param  node     Node that calls the function
     * @param  logError Log the error
     * @return          FunctionSymbol*, nullptr on error
     */
    FunctionSymbol* findFunction(const std::string& name, ast::ASTNode* node,
                                 bool logError = true);

    /**
     * Get the ASTFunctionPrototypeStatement of an ASTNode.
     * Searches parents recursively. Requires a ASTParentSolverVisitor run.
     * @return ASTFunctionPrototypeStatement*, or nullptr on error
     */
    ast::ASTFunctionPrototypeStatement*
    getASTNodeFunction(ast::ASTNode* node) const;

    /**
     * Declare a function.
     * Doesn't redeclare if a function with similar name and type is already
     * declared.
     * Generates code for the prototype.
     * @param  type  FunctionType of function
     * @param  name  Name of function
     * @param  proto ASTFunctionPrototypeStatement of function
     * @return       Created FunctionSymbol, or nullptr on error
     */
    FunctionSymbol* declareFunction(FunctionType* type, const std::string& name,
                                    ast::ASTFunctionPrototypeStatement* proto);

    /**
     * Remove all instructions after block terminators.
     * Also add 'unreachable'-instruction if no terminators are found
     */
    void stripInstructionsAfterTerminators();

    /// Create 'alloca'-instruction for variable
    /**
     * Create 'alloca'-instruction in the function entry block for a variable
     * @param  func Function of variable
     * @param  type Variable type
     * @param  name Variable name
     * @return      Alloca instruction, never nullptr
     */
    llvm::AllocaInst* createEntryBlockAlloca(llvm::Function* func,
                                             llvm::Type* type,
                                             const std::string& name);

    /**
     * Run type infersion on a variable definition based on its initializer.
     * If type information is given otherwise in the definition, it is used
     * instead.
     * Generates code for the init expression.
     * @param expr Variable definition
     * @return Pair of Type (type of variable) and TypedValue (visited init
     * expression). On error both members are nullptr.
     */
    std::pair<Type*, std::unique_ptr<TypedValue>>
    inferVariableDefType(ast::ASTVariableDefinitionExpression* expr);

    /// LLVM context
    llvm::LLVMContext& context;
    /// LLVM module
    llvm::Module* module;
    CodegenInfo info;
    /// LLVM IR builder
    llvm::IRBuilder<> builder;

    /// LLVM debug info builder
    llvm::DIBuilder dbuilder;
    /// LLVM debug compilation unit
    llvm::DICompileUnit* dcu;
    /// LLVM debug file
    llvm::DIFile* dfile;
    /// LLVM debug scopes
    std::vector<llvm::DIScope*> dblocks;

    /// Symbol table
    std::unique_ptr<SymbolTable> symbols;
    /// Type table
    std::unique_ptr<TypeTable> types;

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

    std::unique_ptr<TypedValue> visit(ast::ASTBinaryExpression* expr);
    std::unique_ptr<TypedValue> visit(ast::ASTUnaryExpression* expr);
    std::unique_ptr<TypedValue> visit(ast::ASTAssignmentExpression* node);
    std::unique_ptr<TypedValue> visit(ast::ASTArbitraryOperandExpression* node);

    std::unique_ptr<TypedValue> visit(ast::ASTEmptyStatement* node);
    std::unique_ptr<TypedValue> visit(ast::ASTBlockStatement* node);
    std::unique_ptr<TypedValue> visit(ast::ASTWrappedExpressionStatement* node);
};

inline std::unique_ptr<TypedValue> CodegenVisitor::createVoidVal(llvm::Value* v)
{
    static auto t = types->findDecorated("void");
    assert(t);
    return std::make_unique<TypedValue>(t, v, TypedValue::STMTVALUE, false);
}

inline llvm::Value* CodegenVisitor::getDummyValue()
{
    return getTypedDummyValue()->value;
}

inline std::unique_ptr<TypedValue> CodegenVisitor::getTypedDummyValue()
{
    static auto t = types->findDecorated("int32");
    assert(t);
    auto v = llvm::Constant::getNullValue(t->type);
    auto ret = std::make_unique<TypedValue>(t, v, TypedValue::STMTVALUE, false);
    return ret;
}

template <typename... Args>
inline std::nullptr_t CodegenVisitor::codegenError(ast::ASTNode* node,
                                                   const std::string& format,
                                                   Args&&... args) const
{
    assert(node);
    return util::logCompilerError(node->loc, format,
                                  std::forward<Args>(args)...);
}

template <typename... Args>
inline void CodegenVisitor::codegenWarning(ast::ASTNode* node,
                                           const std::string& format,
                                           Args&&... args) const
{
    assert(node);
    return util::logCompilerWarning(node->loc, format,
                                    std::forward<Args>(args)...);
}

template <typename... Args>
inline void CodegenVisitor::codegenInfo(ast::ASTNode* node,
                                        const std::string& format,
                                        Args&&... args) const
{
    assert(node);
    return util::logCompilerInfo(node->loc, format,
                                 std::forward<Args>(args)...);
}
} // namespace codegen
