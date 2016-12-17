/*
Copyright (C) 2016 Elias Kosunen

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

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
			Parser(const core::lexer::TokenVector &tok);

			Parser(const Parser&) = delete;
			Parser(Parser&&) = default;
			Parser &operator =(const Parser&) = delete;
			Parser &operator =(Parser&&) = default;
			~Parser() = default;

			void run();

			bool getError() const;
			ErrorLevel getErrorLevel() const;

			ast::AST *getAST();
			ast::AST &getASTRef();
			const ast::AST &getASTConstRef() const;
			std::unique_ptr<ast::AST> retrieveAST();

			bool warningsAsErrors;

		private:
			std::unique_ptr<ast::AST> ast;
			const core::lexer::TokenVector &tokens;
			core::lexer::TokenVector::const_iterator it;
			const core::lexer::TokenVector::const_iterator endTokens;

			ErrorLevel error;

			template <typename... Args>
			std::nullptr_t parserError(const std::string &format, const Args& ... args)
			{
				error = ERROR_ERROR;
				auto loc = [&]()
				{
					if(it == tokens.begin())
					{
						return core::lexer::SourceLocation(it->loc.file, 1);
					}
					return (it - 1)->loc;
				}();
				util::logger->error("{}: Parser error: {}", loc.toString(), fmt::format(format, args...));
				return nullptr;
			}

			template <typename... Args>
			void parserWarning(const std::string &format, const Args& ... args)
			{
				if(error != ERROR_ERROR)
				{
					error = ERROR_WARNING;
				}
				auto loc = [&]()
				{
					if(it == tokens.begin())
					{
						return core::lexer::SourceLocation(it->loc.file, 1);
					}
					return (it - 1)->loc;
				}();
				util::logger->warn("{}: Parser warning: {}", (it - 1)->loc.toString(), fmt::format(format, args...));
			}

			std::vector<std::unique_ptr<ast::ASTStatement>> &getGlobalNodeList()
			{
				return ast->globalNode->nodes;
			}

			bool isPrefixUnaryOperator() const;
			bool isPrefixUnaryOperator(const core::lexer::TokenType &op) const;
			bool isPostfixUnaryOperator() const;
			bool isPostfixUnaryOperator(const core::lexer::TokenType &op) const;
			bool isUnaryOperator() const;
			bool isUnaryOperator(const core::lexer::TokenType &op) const;
			bool isBinaryOperator() const;
			bool isAssignmentOperator() const;
			bool isAssignmentOperator(core::lexer::TokenVector::const_iterator op) const;
			bool isOperator() const;
			int getBinOpPrecedence() const;
			int getBinOpPrecedence(core::lexer::TokenVector::const_iterator op) const;
			int getBinOpPrecedence(const core::lexer::TokenType &t) const;
			bool isBinOpRightAssociative() const;
			bool isBinOpRightAssociative(core::lexer::TokenVector::const_iterator op) const;

			std::tuple<
				std::unique_ptr<ast::ASTExpression>, std::unique_ptr<ast::ASTExpression>, std::unique_ptr<ast::ASTExpression>
			>
				parseForCondition();

			std::unique_ptr<ast::ASTImportStatement> parseImportStatement();
			std::unique_ptr<ast::ASTIfStatement> parseIfStatement();
			std::unique_ptr<ast::ASTForStatement> parseForStatement();
			std::unique_ptr<ast::ASTVariableDefinitionExpression> parseVariableDefinition();
			std::unique_ptr<ast::ASTModuleStatement> parseModuleStatement();
			std::unique_ptr<ast::ASTReturnStatement> parseReturnStatement();

			std::unique_ptr<ast::ASTIntegerLiteralExpression> parseIntegerLiteralExpression();
			std::unique_ptr<ast::ASTFloatLiteralExpression> parseFloatLiteralExpression();
			std::unique_ptr<ast::ASTStringLiteralExpression> parseStringLiteralExpression();
			std::unique_ptr<ast::ASTCharLiteralExpression> parseCharLiteralExpression();
			std::unique_ptr<ast::ASTBoolLiteralExpression> parseTrueLiteralExpression();
			std::unique_ptr<ast::ASTBoolLiteralExpression> parseFalseLiteralExpression();
			std::unique_ptr<ast::ASTNoneLiteralExpression> parseNoneLiteralExpression();

			std::unique_ptr<ast::ASTExpression> parseIdentifierExpression();
			std::unique_ptr<ast::ASTCallExpression> parseFunctionCallExpression(const std::string &idName);
			std::unique_ptr<ast::ASTExpression> parsePrimary(bool tolerateUnrecognized = false);
			std::unique_ptr<ast::ASTExpression> parseExpression();

			void handleImport();
			void handleModule();
			void handleDef();
			void handleEmptyStatement();

			std::unique_ptr<ast::ASTStatement> parseStatement();
			std::unique_ptr<ast::ASTBlockStatement> parseBlockStatement();
			std::unique_ptr<ast::ASTFunctionPrototypeStatement> parseFunctionPrototype();
			std::unique_ptr<ast::ASTFunctionDefinitionStatement> parseFunctionDefinitionStatement();

			std::unique_ptr<ast::ASTWrappedExpressionStatement> wrapExpression(std::unique_ptr<ast::ASTExpression> expr);
			std::unique_ptr<ast::ASTEmptyStatement> emptyStatement()
			{
				++it;
				return std::make_unique<ast::ASTEmptyStatement>();
			}

			void _runParser();
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

		inline ast::AST *Parser::getAST()
		{
			assert(ast && "Trying to access nullptr AST");
			return ast.get();
		}
		inline ast::AST &Parser::getASTRef()
		{
			assert(ast && "Trying to access nullptr AST");
			return *(ast.get());
		}
		inline const ast::AST &Parser::getASTConstRef() const
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
