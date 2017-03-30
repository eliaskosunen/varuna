// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#include "ast/Node.h"
#include "ast/FunctionStmt.h"

namespace ast
{
FunctionDefinitionStmt* Node::_getFunction()
{
    // Get function of current node with recursion upwards the tree

    // If current node is a FunctionDefinitionStmt, return it
    if(nodeType == FUNCTION_DEF_STMT)
    {
        return dynamic_cast<FunctionDefinitionStmt*>(this);
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
} // namespace ast
