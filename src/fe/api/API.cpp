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

#include "fe/api/API.h"

#include "util/FileCache.h"
#include "core/parser/Parser.h"
#include "core/codegen/Codegen.h"

namespace fe
{
	namespace api
	{
		static util::FileCache *fileCache()
		{
			static auto cache = std::make_unique<util::FileCache>();
			return cache.get();
		}

		void setLoggingLevel(LoggingLevel l)
		{
			spdlog::set_level(static_cast<spdlog::level::level_enum>(l));
		}

		bool addFile(const std::string &file)
		{
			if(!fileCache()->addFile(file))
			{
				return false;
			}
			util::logger->trace("Added file to cache: '{}'", file);
			return true;
		}

		std::unique_ptr<core::lexer::TokenVector> lexer(const std::string &file)
		{
			auto f = fileCache()->getFile(file);
			core::lexer::Lexer l(f->content, file);
			auto t = std::make_unique<core::lexer::TokenVector>(l.run());
			if(l.getError())
			{
				util::logger->info("Lexing of file '{}' failed, terminating", file);
				return nullptr;
			}
			return t;
		}

		std::unique_ptr<core::ast::AST> parse(core::lexer::TokenVector *tokens)
		{
			core::parser::Parser p(*tokens);
			p.run();
			if(p.getError())
			{
				util::logger->info("Parsing of file '{}' failed, terminating", (*tokens)[0].loc.file);
				return nullptr;
			}
			return p.retrieveAST();
		}

		bool codegen(std::unique_ptr<core::ast::AST> ast)
		{
			auto codegen = std::make_unique<core::codegen::Codegen>(std::move(ast));
			return codegen->run();
		}

		bool runFile(const std::string &file)
		{
			util::logger->info("Running file: '{}'", file);

			util::logger->debug("Starting lexer");
			auto tokens = lexer(file);
			if(!tokens)
			{
				util::logger->debug("Lexing failed\n");
				return false;
			}
			util::logger->debug("Lexing finished\n");

			util::logger->debug("Starting parser");
			auto ast = parse(tokens.get());
			if(!ast)
			{
				util::logger->debug("Parsing failed\n");
				return false;
			}
			util::logger->debug("Parsing finished\n");

			core::ast::DumpASTVisitor::dump<core::ast::ASTBlockStatement>(ast->globalNode.get());

			util::logger->debug("Starting code generation");
			if(!codegen(std::move(ast)))
			{
				util::logger->debug("Code generation failed");
				return false;
			}
			util::logger->debug("Code generation finished");

			return true;
		}
	}
}
