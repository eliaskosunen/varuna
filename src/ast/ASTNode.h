// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#pragma once

#include "ast/FwdDecl.h"
#include "util/SafeEnum.h"
#include "util/SourceLocation.h"
#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>

namespace ast
{
/// A node in the AST
class ASTNode
{
public:
    /// Node type
    enum _NodeType
    {
        NODE = -1,

        EXPR = 0,
        ARBITRARY_OPERAND_EXPR,
        ASSIGNMENT_EXPR,
        BINARY_EXPR,
        BOOL_LITERAL_EXPR,
        CAST_EXPR = 5,
        CHAR_LITERAL_EXPR,
        EMPTY_EXPR,
        FLOAT_LITERAL_EXPR,
        IDENTIFIER_EXPR,
        VARIABLE_REF_EXPR = 10,
        INTEGER_LITERAL_EXPR,
        MEMBER_ACCESS_EXPR,
        NONE_LITERAL_EXPR,
        STRING_LITERAL_EXPR,
        SUBSCRIPT_EXPR = 15,
        SUBSCRIPT_RANGED_EXPR,
        UNARY_EXPR,
        VARIABLE_DEFINITION_EXPR,
        GLOBAL_VARIABLE_DEFINITION_EXPR,

        STMT = 100,
        ALIAS_STMT,
        BLOCK_STMT,
        EMPTY_STMT,
        FOREACH_STMT,
        FOR_STMT = 105,
        FUNCTION_DEF_STMT,
        FUNCTION_PARAMETER,
        FUNCTION_PROTO_STMT,
        IF_STMT,
        IMPORT_STMT = 110,
        MODULE_STMT,
        RETURN_STMT,
        WHILE_STMT,
        WRAPPED_EXPR_STMT
    };
    using NodeType = util::SafeEnum<_NodeType>;

    ASTNode() = default;
    ASTNode(const ASTNode&) = delete;
    ASTNode& operator=(const ASTNode&) = delete;
    ASTNode(ASTNode&&) = default;
    ASTNode& operator=(ASTNode&&) = default;
    virtual ~ASTNode() noexcept = default;

    /**
     * Get the ASTFunctionDefinitionStatement of the node
     * Requires a ASTParentSolverVisitor beforehand
     * @return Pointer to the function, or nullptr if none was found (e.g.
     * Global node without a function)
     */
    ASTFunctionDefinitionStatement* getFunction()
    {
        return _getFunction();
    }

    virtual void accept(DumpASTVisitor* v, size_t ind = 0) = 0;
    virtual void accept(ASTParentSolverVisitor* v, ASTNode* p) = 0;
    virtual bool accept(codegen::GrammarCheckerVisitor* v) = 0;
    virtual std::unique_ptr<codegen::TypedValue>
    accept(codegen::CodegenVisitor* v) = 0;

    /// NodeType of this ASTNode
    NodeType nodeType{NODE};
    /// The parent of this ASTNode
    /// nullptr by default, run ASTParentSolverVisitor to assign a value
    /// If this is nullptr after parent solving, this is a root node
    ASTNode* parent{nullptr};
    /// Is ASTNode marked to be an exported symbol
    bool isExport{false};
    /// Location of this file in the source code
    util::SourceLocation loc;
    /// Tree of this node
    /// nullptr by default, is not set automatically, set yourself after
    /// creation
    AST* ast{nullptr};

protected:
    /// Constructor for child nodes
    explicit ASTNode(NodeType t) : nodeType(t)
    {
    }

    /// Actual implementation of getFunction()
    ASTFunctionDefinitionStatement* _getFunction();
};
} // namespace ast
