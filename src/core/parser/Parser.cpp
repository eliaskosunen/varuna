// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#include "core/parser/Parser.h"
#include "ast/ParentSolverVisitor.h"

namespace core
{
namespace parser
{
    using namespace lexer;
    using namespace ast;

    Parser::Parser(std::shared_ptr<util::File> f, const lexer::TokenVector& tok)
        : warningsAsErrors(false), ast(std::make_unique<ast::AST>(f)),
          tokens(tok), it(tokens.begin()), endTokens(tokens.end()),
          error(ERROR_NONE), file(f)
    {
    }

    void Parser::run()
    {
        _runParser();

        auto parentSolver = std::make_unique<ParentSolverVisitor>();
        parentSolver->run<BlockStmt>(ast->globalNode.get());
    }

    void Parser::_runParser()
    {
        while(true)
        {
            // If reached end of token list, throw
            // This should not be possible, EOF should be reached first
            if(it == endTokens)
            {
                throw std::logic_error("EOF not in the end of the token list");
            }

            // Check current token
            switch(it->type.get())
            {
            // In case of EOF, stop
            case TOKEN_EOF:
                return;

            // A lonely semicolon top-level token
            case TOKEN_PUNCT_SEMICOLON:
                // This means an empty statement
                // That's a warning
                parserWarning("Empty statement");
                handleEmptyStatement(); // Skips the ';'
                break;

            // Import statement
            case TOKEN_KEYWORD_IMPORT:
                handleImport();
                break;
            // Module statement
            case TOKEN_KEYWORD_MODULE:
                handleModule();
                break;
            // Function definition
            case TOKEN_KEYWORD_DEFINE:
                handleDef();
                break;
            // Global variable
            case TOKEN_KEYWORD_LET:
                handleGlobalVariable();
                break;
            // Exported symbol
            case TOKEN_KEYWORD_EXPORT:
                handleExport();
                break;
            // Alias
            case TOKEN_KEYWORD_USE:
                handleUse();
                break;

            // Unsupported top-level token
            default:
                parserError(it, "'{}' is not allowed as a top-level token",
                            it->value);
                return;
            }
        }
    }

    bool Parser::getError() const
    {
        if(error == ERROR_NONE)
        {
            return false;
        }
        if(warningsAsErrors)
        {
            return true;
        }
        if(error == ERROR_ERROR)
        {
            return true;
        }
        return false;
    }

    ErrorLevel Parser::getErrorLevel() const
    {
        return error;
    }

    ast::AST& Parser::getAST()
    {
        assert(ast && "Trying to access nullptr AST");
        return *(ast.get());
    }
    const ast::AST& Parser::getAST() const
    {
        assert(ast && "Trying to access nullptr AST");
        return *(ast.get());
    }
    std::unique_ptr<ast::AST> Parser::retrieveAST()
    {
        assert(ast && "Trying to access nullptr AST");
        return std::move(ast);
    }

    bool Parser::isPrefixUnaryOperator() const
    {
        return isPrefixUnaryOperator(it->type.convert<util::OperatorType>());
    }

    bool Parser::isPrefixUnaryOperator(util::OperatorType op) const
    {
        return (op == util::OPERATORU_PLUS || op == util::OPERATORU_MINUS ||
                op == util::OPERATORU_NOT || op == util::OPERATORU_SIZEOF ||
                op == util::OPERATORU_TYPEOF || op == util::OPERATORU_ADDROF);
    }

    bool Parser::isPostfixUnaryOperator() const
    {
        return isPostfixUnaryOperator(it->type.convert<util::OperatorType>());
    }

    bool Parser::isPostfixUnaryOperator(util::OperatorType op) const
    {
        return (op == util::OPERATORU_INC || op == util::OPERATORU_DEC);
    }

    bool Parser::isUnaryOperator() const
    {
        return isUnaryOperator(it->type.convert<util::OperatorType>());
    }

    bool Parser::isUnaryOperator(util::OperatorType op) const
    {
        return isPrefixUnaryOperator(op) || isPostfixUnaryOperator(op);
    }

    bool Parser::isBinaryOperator() const
    {
        return getBinOpPrecedence() >= 10;
    }

    bool Parser::isOperator() const
    {
        return isBinaryOperator() || isUnaryOperator();
    }

    int Parser::getBinOpPrecedence() const
    {
        return getBinOpPrecedence(it->type.convert<util::OperatorType>());
    }

    int Parser::getBinOpPrecedence(util::OperatorType t) const
    {
        using namespace lexer;

        switch(t.get())
        {
        case util::OPERATORB_SUBSCR:
        case util::OPERATORB_MEMBER:
            return 90;
        case util::OPERATORB_AS:
            return 80;
        case util::OPERATORB_MUL:
        case util::OPERATORB_DIV:
        case util::OPERATORB_MOD:
        case util::OPERATORB_REM:
            return 70;
        case util::OPERATORB_ADD:
        case util::OPERATORB_SUB:
            return 60;
        case util::OPERATORB_LESS:
        case util::OPERATORB_LESSEQ:
        case util::OPERATORB_GREATER:
        case util::OPERATORB_GREATEQ:
            return 50;
        case util::OPERATORB_EQ:
        case util::OPERATORB_NOTEQ:
            return 40;
        case util::OPERATORB_AND:
            return 30;
        case util::OPERATORB_OR:
            return 20;
        case util::OPERATORA_SIMPLE:
        case util::OPERATORA_ADD:
        case util::OPERATORA_SUB:
        case util::OPERATORA_MUL:
        case util::OPERATORA_DIV:
        case util::OPERATORA_MOD:
            return 10;
        default:
            return -1;
        }
    }

    bool Parser::isAssignmentOperator() const
    {
        return isAssignmentOperator(it->type.convert<util::OperatorType>());
    }

    bool Parser::isAssignmentOperator(util::OperatorType op) const
    {
        using namespace lexer;

        switch(op.get())
        {
        case TOKEN_OPERATORA_SIMPLE:
        case TOKEN_OPERATORA_ADD:
        case TOKEN_OPERATORA_SUB:
        case TOKEN_OPERATORA_MUL:
        case TOKEN_OPERATORA_DIV:
        case TOKEN_OPERATORA_MOD:
        case TOKEN_OPERATORA_POW:
            return true;
        default:
            return false;
        }
    }

    bool Parser::isBinOpRightAssociative() const
    {
        return isBinOpRightAssociative(it->type.convert<util::OperatorType>());
    }

    bool Parser::isBinOpRightAssociative(util::OperatorType op) const
    {
        return isAssignmentOperator(op);
    }
} // namespace parser
} // namespace core
