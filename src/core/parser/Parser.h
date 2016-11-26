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

#include "core/parser/FwdDecl.h"
#include "core/parser/AST.h"
#include "core/parser/ASTControlStatement.h"
#include "core/lexer/Lexer.h"
#include "util/Logger.h"
#include "util/Compatibility.h"

namespace core
{
	namespace parser
	{
		class Parser
		{
			std::unique_ptr<AST> ast;
			const core::lexer::TokenVector &tokens;
			core::lexer::TokenVector::const_iterator it;
			const core::lexer::TokenVector::const_iterator endTokens;

			std::vector<std::unique_ptr<ASTStatement>> &getGlobalNodeList()
			{
				return ast->globalNode->nodes;
			}

			int getTokenPrecedence() const
			{
				using namespace core::lexer;
				return ([this]() -> int
				{
					switch(it->type.get())
					{
					case TOKEN_OPERATORA_SIMPLE:
					case TOKEN_OPERATORA_ADD:
					case TOKEN_OPERATORA_SUB:
					case TOKEN_OPERATORA_MUL:
					case TOKEN_OPERATORA_DIV:
					case TOKEN_OPERATORA_MOD:
					case TOKEN_OPERATORA_BITAND:
					case TOKEN_OPERATORA_BITOR:
					case TOKEN_OPERATORA_BITXOR:
					case TOKEN_OPERATORA_SHIFTL:
					case TOKEN_OPERATORA_SHIFTR:
						return 10;

					case TOKEN_OPERATORB_OR:
						return 20;
					case TOKEN_OPERATORB_AND:
						return 30;
					case TOKEN_OPERATORB_BITOR:
						return 40;
					case TOKEN_OPERATORB_BITXOR:
						return 50;
					case TOKEN_OPERATORB_BITAND:
						return 60;

					case TOKEN_OPERATORB_EQ:
					case TOKEN_OPERATORB_NOTEQ:
						return 70;
					case TOKEN_OPERATORB_GREATER:
					case TOKEN_OPERATORB_GREATEQ:
					case TOKEN_OPERATORB_LESS:
					case TOKEN_OPERATORB_LESSEQ:
						return 80;

					case TOKEN_OPERATORB_SHIFTL:
					case TOKEN_OPERATORB_SHIFTR:
						return 90;
					case TOKEN_OPERATORB_ADD:
					case TOKEN_OPERATORB_SUB:
						return 100;
					case TOKEN_OPERATORB_MUL:
					case TOKEN_OPERATORB_DIV:
					case TOKEN_OPERATORB_MOD:
						return 110;

					case TOKEN_OPERATORU_NOT:
					case TOKEN_OPERATORU_BITNOT:
					case TOKEN_OPERATORU_PLUS:
					case TOKEN_OPERATORU_MINUS:
					case TOKEN_OPERATORU_SIZEOF:
					case TOKEN_OPERATORU_TYPEOF:
					case TOKEN_OPERATORU_INSTOF:
					case TOKEN_OPERATORU_ADDROF:
					case TOKEN_OPERATORU_DEC:
					case TOKEN_OPERATORU_INC:
						return 120;

					case TOKEN_OPERATORB_MEMBER:
						return 130;

					default:
						return -1;
					}
				})();
			}

			std::unique_ptr<ASTImportStatement> parseImportStatement();
			std::unique_ptr<ASTIfStatement> parseIfStatement();
			std::unique_ptr<ASTForStatement> parseForStatement();
			std::unique_ptr<ASTVariableDefinitionExpression> parseVariableDefinition();
			std::unique_ptr<ASTExpression> parseUnary();

			std::unique_ptr<ASTIntegerLiteralExpression> parseIntegerLiteralExpression();
			std::unique_ptr<ASTFloatLiteralExpression> parseFloatLiteralExpression();
			std::unique_ptr<ASTStringLiteralExpression> parseStringLiteralExpression();
			std::unique_ptr<ASTCharLiteralExpression> parseCharLiteralExpression();
			std::unique_ptr<ASTBoolLiteralExpression> parseTrueLiteralExpression();
			std::unique_ptr<ASTBoolLiteralExpression> parseFalseLiteralExpression();
			std::unique_ptr<ASTNoneLiteralExpression> parseNoneLiteralExpression();

			std::unique_ptr<ASTExpression> parseParenExpression();
			std::unique_ptr<ASTExpression> parseIdentifierExpression();
			std::unique_ptr<ASTExpression> parsePrimary();
			std::unique_ptr<ASTExpression> parseBinaryOperatorRHS(int prec, std::unique_ptr<ASTExpression> lhs);
			std::unique_ptr<ASTExpression> parseExpression();

			void handleImport()
			{
				if(auto import = parseImportStatement())
				{
					ast->pushStatement(std::move(import));
				}
				else
				{
					++it;
				}
			}

			std::unique_ptr<ASTStatement> parseStatement()
			{
				++it;
				return nullptr;
			}
			std::unique_ptr<ASTBlockStatement> parseBlockStatement()
			{
				auto block = std::make_unique<ASTBlockStatement>(parseStatement());
				return block;
			}
		public:
			Parser(const core::lexer::TokenVector &tok) : ast(std::make_unique<AST>()), tokens(tok), it(tokens.begin()), endTokens(tokens.end()) {}

			void run();

			AST *getAST() { return ast.get(); }
			std::unique_ptr<AST> retrieveAST() { return std::move(ast); }
		};
	}
}
