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
#include "util/Logger.h"
#include "util/Compatibility.h"

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

		class Parser
		{
			std::unique_ptr<ast::AST> ast;
			const core::lexer::TokenVector &tokens;
			core::lexer::TokenVector::const_iterator it;
			const core::lexer::TokenVector::const_iterator endTokens;

			ErrorLevel error;

			template <typename... Args>
			std::nullptr_t parserError(const std::string &format, const Args& ... args)
			{
				error = ERROR_ERROR;
				util::logger->error("Parser error: {}", fmt::format(format, args...));
				return nullptr;
			}

			template <typename... Args>
			void parserWarning(const std::string &format, const Args& ... args)
			{
				if(error != ERROR_ERROR) error = ERROR_WARNING;
				util::logger->warn("Parser warning: {}", fmt::format(format, args...));
			}

			std::vector<std::unique_ptr<ast::ASTStatement>> &getGlobalNodeList()
			{
				return ast->globalNode->nodes;
			}

			int getTokenPrecedence() const;

			std::unique_ptr<ast::ASTImportStatement> parseImportStatement();
			std::unique_ptr<ast::ASTIfStatement> parseIfStatement();
			std::unique_ptr<ast::ASTForStatement> parseForStatement();
			std::unique_ptr<ast::ASTVariableDefinitionExpression> parseVariableDefinition();
			std::unique_ptr<ast::ASTModuleStatement> parseModuleStatement();

			std::unique_ptr<ast::ASTExpression> parseUnary();

			std::unique_ptr<ast::ASTIntegerLiteralExpression> parseIntegerLiteralExpression();
			std::unique_ptr<ast::ASTFloatLiteralExpression> parseFloatLiteralExpression();
			std::unique_ptr<ast::ASTStringLiteralExpression> parseStringLiteralExpression();
			std::unique_ptr<ast::ASTCharLiteralExpression> parseCharLiteralExpression();
			std::unique_ptr<ast::ASTBoolLiteralExpression> parseTrueLiteralExpression();
			std::unique_ptr<ast::ASTBoolLiteralExpression> parseFalseLiteralExpression();
			std::unique_ptr<ast::ASTNoneLiteralExpression> parseNoneLiteralExpression();

			std::unique_ptr<ast::ASTExpression> parseParenExpression();
			std::unique_ptr<ast::ASTExpression> parseIdentifierExpression();
			std::unique_ptr<ast::ASTExpression> parsePrimary();
			std::unique_ptr<ast::ASTExpression> parseBinaryOperatorRHS(int prec, std::unique_ptr<ast::ASTExpression> lhs);
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
		public:
			bool warningsAsErrors;

			Parser(const core::lexer::TokenVector &tok) : ast(std::make_unique<ast::AST>()), tokens(tok), it(tokens.begin()), endTokens(tokens.end()), error(ERROR_NONE), warningsAsErrors(false) {}

			void run();

			bool getError() const
			{
				if(error == ERROR_NONE) return false;
				if(warningsAsErrors) return true;
				if(error == ERROR_ERROR) return true;
				return false;
			}

			ErrorLevel getErrorLevel() const
			{
				return error;
			}

			ast::AST *getAST()
			{
				assert(ast && "Trying to access nullptr AST");
				return ast.get();
			}
			ast::AST &getASTRef()
			{
				assert(ast && "Trying to access nullptr AST");
				return *(ast.get());
			}
			const ast::AST &getASTConstRef() const
			{
				assert(ast && "Trying to access nullptr AST");
				return *(ast.get());
			}
			std::unique_ptr<ast::AST> retrieveAST()
			{
				assert(ast && "Trying to access nullptr AST");
				return std::move(ast);
			}
		};
	}
}
