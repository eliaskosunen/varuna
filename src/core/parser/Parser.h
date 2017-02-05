// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#pragma once

#include "ast/AST.h"
#include "ast/DumpASTVisitor.h"
#include "core/lexer/Lexer.h"
#include "util/File.h"
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
        Parser(std::shared_ptr<util::File> f, const lexer::TokenVector& tok);

        Parser(const Parser&) = delete;
        Parser(Parser&&) = default;
        Parser& operator=(const Parser&) = delete;
        Parser& operator=(Parser&&) = default;
        ~Parser() noexcept = default;

        /// Run the parser
        void run();

        bool getError() const;
        ErrorLevel getErrorLevel() const;

        /// Get reference to the AST
        ast::AST& getAST();
        const ast::AST& getAST() const;
        /// Consume the AST
        std::unique_ptr<ast::AST> retrieveAST();

        bool warningsAsErrors;

    private:
        template <typename... Args>
        std::nullptr_t parserError(const std::string& format, Args&&... args)
        {
            error = ERROR_ERROR;
            auto loc = [&]() {
                if(it == tokens.begin())
                {
                    return util::SourceLocation(it->loc.file, 1, 1);
                }
                return (it - 1)->loc;
            }();
            util::logger->error("{}: {}", loc.toString(),
                                fmt::format(format, args...));
            return nullptr;
        }

        template <typename... Args>
        void parserWarning(const std::string& format, Args&&... args)
        {
            if(error != ERROR_ERROR)
            {
                error = ERROR_WARNING;
            }
            auto loc = [&]() {
                if(it == tokens.begin())
                {
                    return util::SourceLocation(it->loc.file, 1, 1);
                }
                return (it - 1)->loc;
            }();
            util::logger->warn("{}: {}", (it - 1)->loc.toString(),
                               fmt::format(format, args...));
        }

        std::vector<std::unique_ptr<ast::ASTStatement>>& getGlobalNodeList()
        {
            return ast->globalNode->nodes;
        }

        template <typename T, typename... Args>
        std::unique_ptr<T> createNode(lexer::TokenVector::const_iterator iter,
                                      Args&&... args)
        {
            auto node = std::make_unique<T>(std::forward<Args>(args)...);
            node->loc = iter->loc;
            return node;
        }

        bool isPrefixUnaryOperator() const;
        bool isPrefixUnaryOperator(util::OperatorType op) const;
        bool isPostfixUnaryOperator() const;
        bool isPostfixUnaryOperator(util::OperatorType op) const;
        bool isUnaryOperator() const;
        bool isUnaryOperator(util::OperatorType op) const;
        bool isBinaryOperator() const;
        bool isAssignmentOperator() const;
        bool isAssignmentOperator(lexer::TokenVector::const_iterator op) const;
        bool isOperator() const;
        int getBinOpPrecedence() const;
        int getBinOpPrecedence(lexer::TokenVector::const_iterator op) const;
        int getBinOpPrecedence(util::OperatorType t) const;
        bool isBinOpRightAssociative() const;
        bool
        isBinOpRightAssociative(lexer::TokenVector::const_iterator op) const;

        std::tuple<std::unique_ptr<ast::ASTExpression>,
                   std::unique_ptr<ast::ASTExpression>,
                   std::unique_ptr<ast::ASTExpression>>
        parseForCondition();

        std::unique_ptr<ast::ASTImportStatement> parseImportStatement();
        std::unique_ptr<ast::ASTIfStatement> parseIfStatement();
        std::unique_ptr<ast::ASTForStatement> parseForStatement();
        std::unique_ptr<ast::ASTWhileStatement> parseWhileStatement();
        std::unique_ptr<ast::ASTVariableDefinitionExpression>
        parseVariableDefinition();
        std::unique_ptr<ast::ASTGlobalVariableDefinitionExpression>
        parseGlobalVariableDefinition();
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
        void handleGlobalVariable();
        void handleExport();

        std::unique_ptr<ast::ASTStatement> parseStatement();
        std::unique_ptr<ast::ASTBlockStatement> parseBlockStatement();
        std::unique_ptr<ast::ASTFunctionParameter>
        parseFunctionParameter(uint32_t num);
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
        const lexer::TokenVector& tokens;
        lexer::TokenVector::const_iterator it;
        const lexer::TokenVector::const_iterator endTokens;

        ErrorLevel error;

        std::shared_ptr<util::File> file;
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

    inline ast::AST& Parser::getAST()
    {
        assert(ast && "Trying to access nullptr AST");
        return *(ast.get());
    }
    inline const ast::AST& Parser::getAST() const
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
