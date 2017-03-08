// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#pragma once

#include "ast/AST.h"
#include "ast/FunctionStmt.h"
#include "ast/FwdDecl.h"
#include "ast/Node.h"
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
    void emitDebugLocation(ast::Node* node);

    llvm::DIScope* getTopDebugScope() const;

    std::string getModuleFilename() const;
    std::string getModuleFilename(const std::string& moduleName) const;

    void writeExports(std::unique_ptr<SymbolTable> exports);

    /// Create a new void-typed value
    std::unique_ptr<TypedValue> createVoidVal(llvm::Value* v = nullptr);
    /// Get a dummy LLVM value
    llvm::Value* getDummyValue();
    /// Get a typed dummy value
    std::unique_ptr<TypedValue> getTypedDummyValue();

    bool importModule(ast::ImportStmt* import);

    /**
     * Log an error
     * @param  node   Errorenous node
     * @param  format Message format
     * @param  args   Format arguments
     * @return        nullptr
     */
    template <typename... Args>
    std::nullptr_t codegenError(ast::Node* node, const std::string& format,
                                Args&&... args) const;
    template <typename... Args>
    void codegenWarning(ast::Node* node, const std::string& format,
                        Args&&... args) const;
    template <typename... Args>
    void codegenInfo(ast::Node* node, const std::string& format,
                     Args&&... args) const;

    /**
     * Find a function by name
     * @param  name     Function name
     * @param  node     Node that calls the function
     * @param  logError Log the error
     * @return          FunctionSymbol*, nullptr on error
     */
    FunctionSymbol* findFunction(const std::string& name, ast::Node* node,
                                 bool logError = true);

    /**
     * Get the FunctionPrototypeStmt of an Node.
     * Searches parents recursively. Requires a ParentSolverVisitor run.
     * @return FunctionPrototypeStmt*, or nullptr on error
     */
    ast::FunctionPrototypeStmt* getNodeFunction(ast::Node* node) const;

    /**
     * Declare a function.
     * Doesn't redeclare if a function with similar name and type is already
     * declared.
     * Generates code for the prototype.
     * @param  type  FunctionType of function
     * @param  name  Name of function
     * @param  proto FunctionPrototypeStmt of function
     * @return       Created FunctionSymbol, or nullptr on error
     */
    FunctionSymbol* declareFunction(FunctionType* type, const std::string& name,
                                    ast::FunctionPrototypeStmt* proto);

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
    inferVariableDefType(ast::VariableDefinitionExpr* expr);

    std::string mangleFunctionName(const std::string& name) const;

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
    std::unique_ptr<TypedValue> visit(ast::Node* node) = delete;
    std::unique_ptr<TypedValue> visit(ast::Stmt* node);
    std::unique_ptr<TypedValue> visit(ast::Expr* node);

    std::unique_ptr<TypedValue> visit(ast::IfStmt* node);
    std::unique_ptr<TypedValue> visit(ast::ForStmt* node);
    std::unique_ptr<TypedValue> visit(ast::ForeachStmt* node);
    std::unique_ptr<TypedValue> visit(ast::WhileStmt* node);
    std::unique_ptr<TypedValue> visit(ast::ImportStmt* node);
    std::unique_ptr<TypedValue> visit(ast::ModuleStmt* node);

    std::unique_ptr<TypedValue> visit(ast::EmptyExpr* node);
    std::unique_ptr<TypedValue> visit(ast::IdentifierExpr* node);
    std::unique_ptr<TypedValue> visit(ast::VariableRefExpr* expr);
    std::unique_ptr<TypedValue> visit(ast::VariableDefinitionExpr* expr);
    std::unique_ptr<TypedValue> visit(ast::GlobalVariableDefinitionExpr* expr);

    std::unique_ptr<TypedValue> visit(ast::FunctionParameter* node);
    std::unique_ptr<TypedValue> visit(ast::FunctionPrototypeStmt* node);
    std::unique_ptr<TypedValue> visit(ast::FunctionDefinitionStmt* node);
    std::unique_ptr<TypedValue> visit(ast::ReturnStmt* node);

    std::unique_ptr<TypedValue> visit(ast::IntegerLiteralExpr* expr);
    std::unique_ptr<TypedValue> visit(ast::FloatLiteralExpr* expr);
    std::unique_ptr<TypedValue> visit(ast::StringLiteralExpr* expr);
    std::unique_ptr<TypedValue> visit(ast::CharLiteralExpr* expr);
    std::unique_ptr<TypedValue> visit(ast::BoolLiteralExpr* expr);

    std::unique_ptr<TypedValue> visit(ast::BinaryExpr* expr);
    std::unique_ptr<TypedValue> visit(ast::UnaryExpr* expr);
    std::unique_ptr<TypedValue> visit(ast::AssignmentExpr* node);
    std::unique_ptr<TypedValue> visit(ast::ArbitraryOperandExpr* node);

    std::unique_ptr<TypedValue> visit(ast::EmptyStmt* node);
    std::unique_ptr<TypedValue> visit(ast::BlockStmt* node);
    std::unique_ptr<TypedValue> visit(ast::ExprStmt* node);
    std::unique_ptr<TypedValue> visit(ast::AliasStmt* node);
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
    static auto t = types->findDecorated("i32");
    assert(t);
    auto v = llvm::Constant::getNullValue(t->type);
    auto ret = std::make_unique<TypedValue>(t, v, TypedValue::STMTVALUE, false);
    return ret;
}

template <typename... Args>
inline std::nullptr_t CodegenVisitor::codegenError(ast::Node* node,
                                                   const std::string& format,
                                                   Args&&... args) const
{
    assert(node);
    return util::logCompilerError(node->loc, format,
                                  std::forward<Args>(args)...);
}

template <typename... Args>
inline void CodegenVisitor::codegenWarning(ast::Node* node,
                                           const std::string& format,
                                           Args&&... args) const
{
    assert(node);
    return util::logCompilerWarning(node->loc, format,
                                    std::forward<Args>(args)...);
}

template <typename... Args>
inline void CodegenVisitor::codegenInfo(ast::Node* node,
                                        const std::string& format,
                                        Args&&... args) const
{
    assert(node);
    return util::logCompilerInfo(node->loc, format,
                                 std::forward<Args>(args)...);
}
} // namespace codegen
