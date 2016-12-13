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

#include "fe/api/FwdDecl.h"
#include "util/Logger.h"
#include "core/lexer/Lexer.h"

namespace fe
{
	namespace api
	{
		enum LoggingLevel
		{
			LOG_TRACE = spdlog::level::trace,
			LOG_DEBUG = spdlog::level::debug,
			LOG_INFO = spdlog::level::info
		};

		void setLoggingLevel(LoggingLevel l);

		bool addFile(const std::string &file);

		std::unique_ptr<core::lexer::TokenVector> lexer(const std::string &file);
		std::unique_ptr<core::ast::AST> parse(core::lexer::TokenVector *tokens);
		bool codegen(std::unique_ptr<core::ast::AST> ast);

		bool runFile(const std::string &file);
	} // namespace api
} // namespace fe
