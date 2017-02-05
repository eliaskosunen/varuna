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
class ASTNode
{
public:
    enum NodeType_t
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
        BLOCK_STMT,
        EMPTY_STMT,
        FOREACH_STMT,
        FOR_STMT,
        FUNCTION_DEF_STMT = 105,
        FUNCTION_PARAMETER,
        FUNCTION_PROTO_STMT,
        IF_STMT,
        IMPORT_STMT,
        MODULE_STMT = 110,
        RETURN_STMT,
        WHILE_STMT,
        WRAPPED_EXPR_STMT
    };

    using NodeType = util::SafeEnum<NodeType_t>;

    ASTNode() = default;
    ASTNode(const ASTNode&) = delete;
    ASTNode& operator=(const ASTNode&) = delete;
    ASTNode(ASTNode&&) = default;
    ASTNode& operator=(ASTNode&&) = default;
    virtual ~ASTNode() noexcept = default;

    ASTFunctionDefinitionStatement* getFunction()
    {
        return _getFunction();
    }

    virtual void accept(DumpASTVisitor* v, size_t ind = 0) = 0;
    virtual void accept(ASTParentSolverVisitor* v, ASTNode* p) = 0;
    virtual bool accept(codegen::GrammarCheckerVisitor* v) = 0;

    NodeType nodeType{NODE};
    ASTNode* parent{nullptr};
    bool isExport{false};
    util::SourceLocation loc;
    AST* ast{nullptr};

protected:
    explicit ASTNode(NodeType t) : nodeType(t)
    {
    }

    ASTFunctionDefinitionStatement* _getFunction();
};
} // namespace ast
