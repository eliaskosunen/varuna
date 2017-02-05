// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#include "ast/ASTNode.h"
#include "ast/ASTFunctionStatement.h"

namespace ast
{
ASTFunctionDefinitionStatement* ASTNode::_getFunction()
{
    if(nodeType == FUNCTION_DEF_STMT)
    {
        return static_cast<ASTFunctionDefinitionStatement*>(this);
    }
    if(!parent)
    {
        return nullptr;
    }
    return parent->_getFunction();
}
}
