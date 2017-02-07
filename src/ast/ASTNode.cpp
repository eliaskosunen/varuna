// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#include "ast/ASTNode.h"
#include "ast/ASTFunctionStatement.h"

namespace ast
{
ASTFunctionDefinitionStatement* ASTNode::_getFunction()
{
    // Get function of current node with recursion upwards the tree

    // If current node is a ASTFunctionDefinitionStatement, return it
    if(nodeType == FUNCTION_DEF_STMT)
    {
        return static_cast<ASTFunctionDefinitionStatement*>(this);
    }
    // If no parent is set (nullptr), this is either a root node or parents
    // haven't been solved
    // Either way, return nullptr
    if(!parent)
    {
        return nullptr;
    }

    // This node is not a function, check if the parent node is one
    return parent->_getFunction();
}
}
