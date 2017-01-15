// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#pragma once

#include "core/ast/AST.h"
#include "core/ast/DumpASTVisitor.h"
#include "core/lexer/Lexer.h"
#include "util/Compatibility.h"
#include "util/Logger.h"
#include <tuple>

namespace core
{
namespace parser
{
    enum ErrorLevel
    {
        ERROR_NONE = 0,
        ERROR_WARNING,
        ERROR_ERROR
    };

    class Parser final
    {
    public:
        explicit Parser(const core::lexer::TokenVector& tok);

        Parser(const Parser&) = delete;
        Parser(Parser&&) noexcept = default;
        Parser& operator=(const Parser&) = delete;
        Parser& operator=(Parser&&) noexcept = default;
        ~Parser() noexcept = default;

        void run();

        bool getError() const;
        ErrorLevel getErrorLevel() const;

        ast::AST* getAST();
        ast::AST& getASTRef();
        const ast::AST& getASTConstRef() const;
        std::unique_ptr<ast::AST> retrieveAST();

        bool warningsAsErrors;

    private:
        template <typename... Args>
        std::nullptr_t parserError(const std::string& format,
                                   const Args&... args)
        {
            error = ERROR_ERROR;
            auto loc = [&]() {
                if(it == tokens.begin())
                {
                    return core::lexer::SourceLocation(it->loc.file, 1);
                }
                return (it - 1)->loc;
            }();
            util::logger->error("{}: Parser error: {}", loc.toString(),
                                fmt::format(format, args...));
            return nullptr;
        }

        template <typename... Args>
        void parserWarning(const std::string& format, const Args&... args)
        {
            if(error != ERROR_ERROR)
            {
                error = ERROR_WARNING;
            }
            auto loc = [&]() {
                if(it == tokens.begin())
                {
                    return core::lexer::SourceLocation(it->loc.file, 1);
                }
                return (it - 1)->loc;
            }();
            util::logger->warn("{}: Parser warning: {}",
                               (it - 1)->loc.toString(),
                               fmt::format(format, args...));
        }

        std::vector<std::unique_ptr<ast::ASTStatement>>& getGlobalNodeList()
        {
            return ast->globalNode->nodes;
        }

        bool isPrefixUnaryOperator() const;
        bool isPrefixUnaryOperator(const core::lexer::TokenType& op) const;
        bool isPostfixUnaryOperator() const;
        bool isPostfixUnaryOperator(const core::lexer::TokenType& op) const;
        bool isUnaryOperator() const;
        bool isUnaryOperator(const core::lexer::TokenType& op) const;
        bool isBinaryOperator() const;
        bool isAssignmentOperator() const;
        bool
        isAssignmentOperator(core::lexer::TokenVector::const_iterator op) const;
        bool isOperator() const;
        int getBinOpPrecedence() const;
        int
        getBinOpPrecedence(core::lexer::TokenVector::const_iterator op) const;
        int getBinOpPrecedence(const core::lexer::TokenType& t) const;
        bool isBinOpRightAssociative() const;
        bool isBinOpRightAssociative(
            core::lexer::TokenVector::const_iterator op) const;

        std::tuple<std::unique_ptr<ast::ASTExpression>,
                   std::unique_ptr<ast::ASTExpression>,
                   std::unique_ptr<ast::ASTExpression>>
        parseForCondition();

        std::unique_ptr<ast::ASTImportStatement> parseImportStatement();
        std::unique_ptr<ast::ASTIfStatement> parseIfStatement();
        std::unique_ptr<ast::ASTForStatement> parseForStatement();
        std::unique_ptr<ast::ASTVariableDefinitionExpression>
        parseVariableDefinition();
        std::unique_ptr<ast::ASTModuleStatement> parseModuleStatement();
        std::unique_ptr<ast::ASTReturnStatement> parseReturnStatement();

        std::unique_ptr<ast::ASTIntegerLiteralExpression>
        parseIntegerLiteralExpression();
        std::unique_ptr<ast::ASTFloatLiteralExpression>
        parseFloatLiteralExpression();
        std::unique_ptr<ast::ASTStringLiteralExpression>
        parseStringLiteralExpression();
        std::unique_ptr<ast::ASTCharLiteralExpression>
        parseCharLiteralExpression();
        std::unique_ptr<ast::ASTBoolLiteralExpression>
        parseTrueLiteralExpression();
        std::unique_ptr<ast::ASTBoolLiteralExpression>
        parseFalseLiteralExpression();
        std::unique_ptr<ast::ASTNoneLiteralExpression>
        parseNoneLiteralExpression();

        std::unique_ptr<ast::ASTExpression> parseIdentifierExpression();
        std::unique_ptr<ast::ASTArbitraryOperationExpression>
        parseFunctionCallExpression(std::unique_ptr<ast::ASTExpression> lhs);
        std::unique_ptr<ast::ASTSubscriptExpression>
        parseSubscriptExpression(std::unique_ptr<ast::ASTExpression> lhs);
        std::unique_ptr<ast::ASTCastExpression> parseCastExpression();
        std::unique_ptr<ast::ASTExpression>
        parsePrimary(bool tolerateUnrecognized = false);
        std::unique_ptr<ast::ASTExpression> parseExpression();

        void handleImport();
        void handleModule();
        void handleDef();
        void handleEmptyStatement();

        std::unique_ptr<ast::ASTStatement> parseStatement();
        std::unique_ptr<ast::ASTBlockStatement> parseBlockStatement();
        std::unique_ptr<ast::ASTFunctionPrototypeStatement>
        parseFunctionPrototype();
        std::unique_ptr<ast::ASTFunctionDefinitionStatement>
        parseFunctionDefinitionStatement();

        std::unique_ptr<ast::ASTWrappedExpressionStatement>
        wrapExpression(std::unique_ptr<ast::ASTExpression> expr);
        std::unique_ptr<ast::ASTEmptyStatement> emptyStatement(bool skip = true)
        {
            if(skip)
                ++it;
            return std::make_unique<ast::ASTEmptyStatement>();
        }

        void _runParser();

        std::unique_ptr<ast::AST> ast;
        const core::lexer::TokenVector& tokens;
        core::lexer::TokenVector::const_iterator it;
        const core::lexer::TokenVector::const_iterator endTokens;

        ErrorLevel error;
    };

    inline bool Parser::getError() const
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

    inline ErrorLevel Parser::getErrorLevel() const
    {
        return error;
    }

    inline ast::AST* Parser::getAST()
    {
        assert(ast && "Trying to access nullptr AST");
        return ast.get();
    }
    inline ast::AST& Parser::getASTRef()
    {
        assert(ast && "Trying to access nullptr AST");
        return *(ast.get());
    }
    inline const ast::AST& Parser::getASTConstRef() const
    {
        assert(ast && "Trying to access nullptr AST");
        return *(ast.get());
    }
    inline std::unique_ptr<ast::AST> Parser::retrieveAST()
    {
        assert(ast && "Trying to access nullptr AST");
        return std::move(ast);
    }
} // namespace parser
} // namespace core
