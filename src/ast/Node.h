// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#pragma once

#include "ast/FwdDecl.h"
#include "util/SafeEnum.h"
#include "util/SourceLocation.h"
#include <cereal.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>

namespace ast
{
/// A node in the AST
class Node
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
        CHAR_LITERAL_EXPR = 5,
        EMPTY_EXPR,
        FLOAT_LITERAL_EXPR,
        IDENTIFIER_EXPR,
        VARIABLE_REF_EXPR,
        INTEGER_LITERAL_EXPR = 10,
        STRING_LITERAL_EXPR,
        UNARY_EXPR,
        VARIABLE_DEFINITION_EXPR,
        GLOBAL_VARIABLE_DEFINITION_EXPR,

        STMT = 100,
        ALIAS_STMT,
        BLOCK_STMT,
        EMPTY_STMT,
        EXPR_STMT,
        FOREACH_STMT = 105,
        FOR_STMT,
        FUNCTION_DEF_STMT,
        FUNCTION_PARAMETER,
        FUNCTION_PROTO_STMT,
        IF_STMT = 110,
        IMPORT_STMT,
        MODULE_STMT,
        RETURN_STMT,
        WHILE_STMT
    };
    using NodeType = util::SafeEnum<_NodeType>;

    Node() = default;
    Node(const Node&) = delete;
    Node& operator=(const Node&) = delete;
    Node(Node&&) = default;
    Node& operator=(Node&&) = default;
    virtual ~Node() noexcept = default;

    /**
     * Get the FunctionDefinitionStmt of the node
     * Requires a ParentSolverVisitor beforehand
     * @return Pointer to the function, or nullptr if none was found (e.g.
     * Global node without a function)
     */
    FunctionDefinitionStmt* getFunction()
    {
        return _getFunction();
    }

    virtual void accept(DumpVisitor* v, size_t ind = 0) = 0;
    virtual void accept(ParentSolverVisitor* v, Node* p) = 0;
    virtual std::unique_ptr<codegen::TypedValue>
    accept(codegen::CodegenVisitor* v) = 0;

    template <class Archive>
    void serialize(Archive& archive)
    {
        archive(CEREAL_NVP(nodeType), CEREAL_NVP(isExport), CEREAL_NVP(loc));
    }

    /// NodeType of this Node
    NodeType nodeType{NODE};
    /// The parent of this Node
    /// nullptr by default, run ParentSolverVisitor to assign a value
    /// If this is nullptr after parent solving, this is a root node
    Node* parent{nullptr};
    /// Is Node marked to be an exported symbol
    bool isExport{false};
    /// Location of this file in the source code
    util::SourceLocation loc{};
    /// Tree of this node
    /// nullptr by default, is not set automatically, set yourself after
    /// creation
    AST* ast{nullptr};

protected:
    /// Constructor for child nodes
    explicit Node(NodeType t) : nodeType(t)
    {
    }

    /// Actual implementation of getFunction()
    FunctionDefinitionStmt* _getFunction();
};
} // namespace ast
