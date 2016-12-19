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

#include "core/ast/FwdDecl.h"
#include "core/ast/Visitor.h"

#include "util/Logger.h"

namespace core
{
	namespace ast
	{
		class DumpASTVisitor : public Visitor
		{
			std::shared_ptr<spdlog::logger> astlogger;
		public:
			DumpASTVisitor()
				: astlogger(spdlog::stdout_logger_st("DumpAST"))
			{
				astlogger->set_pattern("DumpAST: %v");
				astlogger->flush_on(spdlog::level::warn);
			}

			~DumpASTVisitor() override
			{
				spdlog::drop("DumpAST");
			}

			DumpASTVisitor(const DumpASTVisitor&) = delete;
			DumpASTVisitor(DumpASTVisitor&&) = default;

			DumpASTVisitor &operator =(const DumpASTVisitor&) & = delete;
			DumpASTVisitor &operator =(DumpASTVisitor&&) & = default;

			template <typename... Args>
			void log(const std::string &format, Args... args)
			{
				astlogger->debug(format.c_str(), args...);
			}

			template <typename... Args>
			void log(size_t ind, const std::string &format, Args... args)
			{
				if(ind == 0)
				{
					log(format, args...);
				}
				else
				{
					std::string prefix("\\");
					size_t realind = ind * 2;
					for(size_t i = 1; i < realind; ++i)
					{
						prefix.push_back('-');
					}
					prefix.append(format);
					log(prefix, args...);
				}
			}

			template <typename T>
			void start(T *root)
			{
				util::loggerBasic->trace("");
				log("*** AST DUMP ***");
				auto castedRoot = dynamic_cast<ASTNode*>(root);
				if(!castedRoot)
				{
					throw std::invalid_argument("Invalid root node given to DumpASTVisitor");
				}
				root->accept(this);
			}

			void finish()
			{
				log("*** FINISHED AST DUMP ***");
				astlogger->flush();
			}

			template <typename T>
			static void dump(T *node)
			{
				auto dumpAST = std::make_unique<DumpASTVisitor>();
				dumpAST->start<T>(node);
				dumpAST->finish();
			}

			void visit(ASTNode *node, size_t ind = 0);
			void visit(ASTStatement *stmt, size_t ind = 0);
			void visit(ASTExpression *expr, size_t ind = 0);

			void visit(ASTIfStatement *node, size_t ind = 0);
			void visit(ASTForStatement *node, size_t ind = 0);
			void visit(ASTForeachStatement *node, size_t ind = 0);
			void visit(ASTWhileStatement *node, size_t ind = 0);
			void visit(ASTImportStatement *node, size_t ind = 0);
			void visit(ASTModuleStatement *node, size_t ind = 0);

			void visit(ASTEmptyExpression *node, size_t ind = 0);
			void visit(ASTIdentifierExpression *node, size_t ind = 0);
			void visit(ASTVariableRefExpression *node, size_t ind = 0);
			void visit(ASTCallExpression *node, size_t ind = 0);
			void visit(ASTCastExpression *node, size_t ind = 0);
			void visit(ASTVariableDefinitionExpression *node, size_t ind = 0);

			void visit(ASTFunctionParameter *node, size_t ind = 0);
			void visit(ASTFunctionPrototypeStatement *node, size_t ind = 0);
			void visit(ASTFunctionDefinitionStatement *node, size_t ind = 0);
			void visit(ASTFunctionDeclarationStatement *node, size_t ind = 0);
			void visit(ASTReturnStatement *node, size_t ind = 0);

			void visit(ASTIntegerLiteralExpression *node, size_t ind = 0);
			void visit(ASTFloatLiteralExpression *node, size_t ind = 0);
			void visit(ASTStringLiteralExpression *node, size_t ind = 0);
			void visit(ASTCharLiteralExpression *node, size_t ind = 0);
			void visit(ASTBoolLiteralExpression *node, size_t ind = 0);
			void visit(ASTNoneLiteralExpression *node, size_t ind = 0);

			void visit(ASTBinaryOperationExpression *node, size_t ind = 0);
			void visit(ASTUnaryOperationExpression *node, size_t ind = 0);
			void visit(ASTAssignmentOperationExpression *node, size_t ind = 0);

			void visit(ASTEmptyStatement *node, size_t ind = 0);
			void visit(ASTBlockStatement *node, size_t ind = 0);
			void visit(ASTWrappedExpressionStatement *node, size_t ind = 0);
		};
	} // namespace ast
} // namespace core
