// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#pragma once

#include "ast/AST.h"
#include "core/lexer/Lexer.h"
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
        Parser(Parser&&) noexcept = default;
        Parser& operator=(const Parser&) = delete;
        Parser& operator=(Parser&&) noexcept = delete;
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
        std::nullptr_t parserError(util::SourceLocation loc,
                                   const std::string& format, Args&&... args);
        template <typename... Args>
        void parserWarning(util::SourceLocation loc, const std::string& format,
                           Args&&... args);
        template <typename... Args>
        void parserInfo(util::SourceLocation loc, const std::string& format,
                        Args&&... args);

        template <typename... Args>
        std::nullptr_t parserError(lexer::TokenVector::const_iterator iter,
                                   const std::string& format, Args&&... args);
        template <typename... Args>
        void parserWarning(lexer::TokenVector::const_iterator iter,
                           const std::string& format, Args&&... args);
        template <typename... Args>
        void parserInfo(lexer::TokenVector::const_iterator iter,
                        const std::string& format, Args&&... args);

        template <typename... Args>
        std::nullptr_t parserError(const std::string& format, Args&&... args);
        template <typename... Args>
        void parserWarning(const std::string& format, Args&&... args);

        std::vector<std::unique_ptr<ast::Stmt>>& getGlobalNodeList()
        {
            return ast->globalNode->nodes;
        }

        template <typename T, typename... Args>
        std::unique_ptr<T> createNode(lexer::TokenVector::const_iterator iter,
                                      Args&&... args);
        template <typename T, typename... Args>
        std::unique_ptr<T> createNode(util::SourceLocation loc, Args&&... args);

        bool isPrefixUnaryOperator() const;
        bool isPrefixUnaryOperator(util::OperatorType op) const;
        bool isPostfixUnaryOperator() const;
        bool isPostfixUnaryOperator(util::OperatorType op) const;
        bool isUnaryOperator() const;
        bool isUnaryOperator(util::OperatorType op) const;
        bool isBinaryOperator() const;
        bool isAssignmentOperator() const;
        bool isAssignmentOperator(util::OperatorType op) const;
        bool isOperator() const;
        int getBinOpPrecedence() const;
        int getBinOpPrecedence(util::OperatorType t) const;
        bool isBinOpRightAssociative() const;
        bool isBinOpRightAssociative(util::OperatorType op) const;

        struct ForCondition
        {
            std::unique_ptr<ast::Expr> init;
            std::unique_ptr<ast::Expr> cond;
            std::unique_ptr<ast::Expr> step;
        };
        ForCondition parseForCondition();

        std::unique_ptr<ast::ImportStmt> parseImportStatement();
        std::unique_ptr<ast::IfStmt> parseIfStatement();
        std::unique_ptr<ast::ForStmt> parseForStatement();
        std::unique_ptr<ast::WhileStmt> parseWhileStatement();
        std::unique_ptr<ast::VariableDefinitionExpr> parseVariableDefinition();
        std::unique_ptr<ast::GlobalVariableDefinitionExpr>
        parseGlobalVariableDefinition();
        std::unique_ptr<ast::ModuleStmt> parseModuleStatement();
        std::unique_ptr<ast::ReturnStmt> parseReturnStatement();

        std::unique_ptr<ast::AliasStmt> parseAliasStatement();

        std::unique_ptr<ast::IntegerLiteralExpr>
        parseIntegerLiteralExpression();
        std::unique_ptr<ast::FloatLiteralExpr> parseFloatLiteralExpression();
        std::unique_ptr<ast::StringLiteralExpr> parseStringLiteralExpression();
        std::unique_ptr<ast::CharLiteralExpr> parseCharLiteralExpression();
        std::unique_ptr<ast::BoolLiteralExpr> parseTrueLiteralExpression();
        std::unique_ptr<ast::BoolLiteralExpr> parseFalseLiteralExpression();

        std::unique_ptr<ast::Expr> parseIdentifierExpression();
        std::unique_ptr<ast::ArbitraryOperandExpr>
        parseFunctionCallExpression(std::unique_ptr<ast::Expr> lhs);
        std::unique_ptr<ast::BinaryExpr>
        parseSubscriptExpression(std::unique_ptr<ast::Expr> lhs);
        std::unique_ptr<ast::Expr>
        parsePrimary(bool tolerateUnrecognized = false);
        std::unique_ptr<ast::Expr> parseExpression();

        void handleImport();
        void handleModule();
        void handleDef();
        void handleEmptyStatement();
        void handleGlobalVariable();
        void handleExport();
        void handleUse();

        std::unique_ptr<ast::Stmt> parseStatement();
        std::unique_ptr<ast::BlockStmt> parseBlockStatement();
        std::unique_ptr<ast::FunctionParameter>
        parseFunctionParameter(uint32_t num);
        std::unique_ptr<ast::FunctionPrototypeStmt> parseFunctionPrototype();
        std::unique_ptr<ast::FunctionDefinitionStmt>
        parseFunctionDefinitionStatement();

        std::unique_ptr<ast::ExprStmt>
        createExprStmt(std::unique_ptr<ast::Expr> expr);
        std::unique_ptr<ast::EmptyStmt> emptyStatement(bool skip = true);

        void _runParser();

        std::unique_ptr<ast::AST> ast;
        const lexer::TokenVector& tokens;
        lexer::TokenVector::const_iterator it;
        const lexer::TokenVector::const_iterator endTokens;

        ErrorLevel error;

        std::shared_ptr<util::File> file;
    };

    template <typename T, typename... Args>
    inline std::unique_ptr<T>
    Parser::createNode(lexer::TokenVector::const_iterator iter, Args&&... args)
    {
        auto node = std::make_unique<T>(std::forward<Args>(args)...);
        node->loc = iter->loc;
        node->ast = ast.get();
        return node;
    }
    template <typename T, typename... Args>
    inline std::unique_ptr<T> Parser::createNode(util::SourceLocation loc,
                                                 Args&&... args)
    {
        auto node = std::make_unique<T>(std::forward<Args>(args)...);
        node->loc = loc;
        node->ast = ast.get();
        return node;
    }

    template <typename... Args>
    std::nullptr_t Parser::parserError(util::SourceLocation loc,
                                       const std::string& format,
                                       Args&&... args)
    {
        error = ERROR_ERROR;
        return util::logCompilerError(loc, format, std::forward<Args>(args)...);
    }
    template <typename... Args>
    void Parser::parserWarning(util::SourceLocation loc,
                               const std::string& format, Args&&... args)
    {
        if(error != ERROR_ERROR)
        {
            error = ERROR_WARNING;
        }
        util::logCompilerWarning(loc, format, std::forward<Args>(args)...);
    }
    template <typename... Args>
    void Parser::parserInfo(util::SourceLocation loc, const std::string& format,
                            Args&&... args)
    {
        util::logCompilerInfo(loc, format, std::forward<Args>(args)...);
    }

    template <typename... Args>
    inline std::nullptr_t
    Parser::parserError(lexer::TokenVector::const_iterator iter,
                        const std::string& format, Args&&... args)
    {
        return parserError(iter->loc, format, std::forward<Args>(args)...);
    }
    template <typename... Args>
    inline void Parser::parserWarning(lexer::TokenVector::const_iterator iter,
                                      const std::string& format, Args&&... args)
    {
        parserWarning(iter->loc, format, std::forward<Args>(args)...);
    }

    template <typename... Args>
    inline void Parser::parserInfo(lexer::TokenVector::const_iterator iter,
                                   const std::string& format, Args&&... args)
    {
        parserInfo(iter->loc, format, std::forward<Args>(args)...);
    }

    template <typename... Args>
    inline std::nullptr_t Parser::parserError(const std::string& format,
                                              Args&&... args)
    {
        return parserError(it, format, std::forward<Args>(args)...);
    }

    template <typename... Args>
    inline void Parser::parserWarning(const std::string& format, Args&&... args)
    {
        parserWarning(it, format, std::forward<Args>(args)...);
    }
} // namespace parser
} // namespace core
