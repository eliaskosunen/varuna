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

#include "core/parser/Parser.h"
#include "core/parser/FwdDecl.h"
#include "core/lexer/Token.h"
#include "core/parser/ASTFunctionStatement.h"
#include "core/parser/ASTControlStatement.h"
#include "util/Compatibility.h"

#include <iterator> // For std::next

namespace core
{
	namespace parser
	{
		using namespace core::lexer;

		void Parser::run()
		{
			bool running = true, isExtern = false;
			while(running)
			{
				if(it == endTokens)
				{
					return;
				}
				switch(it->type.get())
				{
				case TOKEN_EOF:
					running = false;
					continue;
				case TOKEN_PUNCT_SEMICOLON:
					util::logger->warn("Empty statement");
					++it;
					break;

				case TOKEN_KEYWORD_IMPORT:
					getGlobalNodeList().push_back(parseImportStatement());
					break;
				case TOKEN_KEYWORD_EXTERN:
					isExtern = true;
					++it;
					break;
				case TOKEN_KEYWORD_DEFINE:
					getGlobalNodeList().push_back(parseFunctionDefinitionStatement(isExtern));
					isExtern = false;
					break;

				default:
					++it;
					break;
				}
			}
		}

		std::unique_ptr<ASTImportStatement> Parser::parseImportStatement()
		{
			++it; // Skip "import"

			ASTImportStatement::ImportType importType = ASTImportStatement::UNSPECIFIED;
			if(it->type == TOKEN_KEYWORD_MODULE)
			{
				importType = ASTImportStatement::MODULE;
				++it;
			}
			else if(it->type == TOKEN_KEYWORD_PACKAGE)
			{
				importType = ASTImportStatement::PACKAGE;
				++it;
			}

			std::string toImport;
			bool isPath = false;
			if(it->type == TOKEN_IDENTIFIER)
			{
				toImport = it->value;
			}
			else if(it->type == TOKEN_LITERAL_STRING)
			{
				isPath = true;
				toImport = it->value;
			}
			else
			{
				// TODO: Handle error
				util::logger->error("Invalid importee: '{}'", it->value);
				return nullptr;
			}

			if(std::next(it)->type != TOKEN_PUNCT_SEMICOLON)
			{
				// TODO: Handle error
				util::logger->error("Expected semicolon after import statement");
				return nullptr;
			}
			++it; // Skip semicolon

			util::logger->trace("Parsed import statement: {}, {}, {}", importType, toImport, isPath);
			auto toImportObj = std::make_unique<ASTIdentifierExpression>(toImport);
			auto stmt = std::make_unique<ASTImportStatement>(importType, std::move(toImportObj), isPath);
			++it;
			return stmt;
		}

		std::unique_ptr<ASTFunctionDefinitionStatement> Parser::parseFunctionDefinitionStatement(bool isExtern)
		{
			++it; // Skip "def"

			if(it->type != TOKEN_IDENTIFIER)
			{
				// TODO: Handle error
				util::logger->error("Invalid function name: '{}'", it->value);
				return nullptr;
			}
			auto functionName = std::make_unique<ASTIdentifierExpression>(it->value);
			++it;

			if(it->type != TOKEN_PUNCT_PAREN_OPEN)
			{
				// TODO: Handle error
				util::logger->error("Expected opening parenthesis on function definition, got '{}' instead", it->value);
				return nullptr;
			}
			++it;
			std::vector<std::unique_ptr<ASTVariableDefinitionStatement>> params;
			while(it->type != TOKEN_PUNCT_PAREN_CLOSE)
			{
				// handleVariableDefinitionStatement()
				++it;
			}
			++it;

			if(it->type != TOKEN_PUNCT_COLON)
			{
				// TODO: Handle error
				util::logger->error("Expected colon on function definition, got '{}' instead", it->value);
				return nullptr;
			}
			++it;
			if(it->type != TOKEN_IDENTIFIER)
			{
				// TODO: Handle error
				util::logger->error("Expected identifier on function definition, got '{}' instead", it->value);
				return nullptr;
			}
			auto returnType = std::make_unique<ASTIdentifierExpression>(it->value);
			++it;

			if(it->type != TOKEN_PUNCT_BRACE_OPEN)
			{
				// TODO: Handle error
				util::logger->error("Expected opening brace on function definition, got '{}' instead", it->value);
				return nullptr;
			}
			while(it->type != TOKEN_PUNCT_BRACE_CLOSE)
			{
				++it;
			}

			auto function = std::make_unique<ASTFunctionDefinitionStatement>(std::move(returnType), std::move(functionName), nullptr, std::move(params));
			return function;
		}
	}
}
