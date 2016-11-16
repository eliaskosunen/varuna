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
			bool running = true;
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

				case TOKEN_KEYWORD_IMPORT:
					getGlobalNodeList().push_back(parseImportStatement());
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
				return nullptr;
			}
			util::logger->trace("Import statement: {}, {}, {}", importType, toImport, isPath);
			auto stmt = util::make_unique<ASTImportStatement>(importType, util::make_unique<ASTIdentifierExpression>(toImport), isPath);
			++it;
			return stmt;
		}
	}
}
