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
#include "core/parser/ASTOperatorExpression.h"
#include "core/parser/ASTLiteralExpression.h"
#include "util/Compatibility.h"
#include "util/StringUtils.h"

#include <iterator> // For std::next
#include <limits>

namespace core
{
	namespace parser
	{
		using namespace core::lexer;

		void Parser::run()
		{
			while(true)
			{
				if(it == endTokens)
				{
					return;
				}
				switch(it->type.get())
				{
				case TOKEN_EOF:
					return;
				case TOKEN_PUNCT_SEMICOLON:
					util::logger->warn("Empty statement");
					++it;
					break;

				case TOKEN_KEYWORD_IMPORT:
					handleImport();
					break;

				default:
					util::logger->error("'{}' is not allowed as a top-level token", it->value);
					return;
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
				while(true)
				{
					toImport += it->value;
					if(std::next(it)->type == TOKEN_EOF ||
						std::next(it)->type == TOKEN_PUNCT_SEMICOLON)
					{
						break;
					}
					if(std::next(it)->type == TOKEN_OPERATORB_MEMBER ||
						std::next(it)->type == TOKEN_IDENTIFIER)
					{
						++it;
						continue;
					}
					util::logger->error("Invalid importee: '{}{}'", toImport, it->value);
					return nullptr;
				}
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

		std::unique_ptr<ASTExpression> Parser::parseParenExpression()
		{
			++it; // Skip '('
			auto v = parseExpression();
			if(!v)
				return nullptr;

			if(it->type != TOKEN_PUNCT_PAREN_CLOSE)
			{
				// TODO: Handle error
				util::logger->error("Expected closing parenthesis, got '{}' instead", it->value);
				return nullptr;
			}
			++it; // Skip ')'
			return v;
		}

		std::unique_ptr<ASTExpression> Parser::parseIdentifierExpression()
		{
			std::string idName = it->value;
			++it; // Skip identifier

			if(it->type == TOKEN_IDENTIFIER || it->type == TOKEN_KEYWORD_VAR)
			{
				--it;
				return parseVariableDefinition();
			}
			if(it->type == TOKEN_PUNCT_PAREN_OPEN)
			{
				// Function call
				++it; // Skip '('
				std::vector<std::unique_ptr<ASTExpression>> args;
				if(it->type != TOKEN_PUNCT_PAREN_CLOSE)
				{
					while(it->type != TOKEN_EOF)
					{
						if(auto arg = parseExpression())
						{
							args.push_back(std::move(arg));
						}
						else
						{
							return nullptr;
						}

						if(it->type == TOKEN_PUNCT_PAREN_CLOSE)
						{
							break;
						}

						if(it->type != TOKEN_PUNCT_COMMA)
						{
							// TODO: Handle error
							util::logger->error("Invalid function call: Expected ')' or ',' in argument list, got '{}' instead", it->value);
							return nullptr;
						}
						++it;
					}
				}

				++it; // Skip ')'
				auto id = std::make_unique<ASTIdentifierExpression>(idName);
				return std::make_unique<ASTCallExpression>(std::move(id), std::move(args));
			}

			// Simple variable reference
			return std::make_unique<ASTVariableRefExpression>(idName);
		}

		std::unique_ptr<ASTIfStatement> Parser::parseIfStatement()
		{
			++it; // Skip 'if'

			if(it->type != TOKEN_PUNCT_PAREN_OPEN)
			{
				// TODO: Handle error
				util::logger->error("Invalid if statement: Expected '(', got '{}' instead", it->value);
				return nullptr;
			}
			auto cond = parseParenExpression();
			if(!cond)
			{
				return nullptr;
			}

			auto then = std::make_unique<ASTBlockStatement>();
			if(it->type != TOKEN_PUNCT_BRACE_OPEN)
			{
				auto tmp = parseStatement();
				if(!tmp)
				{
					// TODO: Handle error
					util::logger->error("Invalid if statement then statement");
					return nullptr;
				}
				then->nodes.push_back(std::move(tmp));
			}
			else
			{
				then = parseBlockStatement();
				if(!then)
				{
					// TODO: Handle error
					util::logger->error("Invalid if statement then statement block");
					return nullptr;
				}
			}

			auto elseBlock = std::make_unique<ASTBlockStatement>();
			if(it->type == TOKEN_KEYWORD_ELSE)
			{
				++it; // Skip else
				if(it->type != TOKEN_PUNCT_BRACE_OPEN)
				{
					auto tmp = parseStatement();
					if(!tmp)
					{
						// TODO: Handle error
						util::logger->error("Invalid if statement else block");
						return nullptr;
					}
					elseBlock->nodes.push_back(std::move(tmp));
				}
				else
				{
					elseBlock = parseBlockStatement();
					if(!elseBlock)
					{
						// TODO: Handle error
						util::logger->error("Invalid if statement else block");
						return nullptr;
					}
				}
			}

			return std::make_unique<ASTIfStatement>(std::move(cond), std::move(then), std::move(elseBlock));
		}

		std::unique_ptr<ASTForStatement> Parser::parseForStatement()
		{
			++it; // Skip 'for'

			if(it->type != TOKEN_PUNCT_PAREN_OPEN)
			{
				// TODO: Handle error
				util::logger->error("Invalid for statement: expected '(' after 'for', got '{}'", it->value);
				return nullptr;
			}
			++it; // Skip '('

			std::unique_ptr<ASTExpression> start = nullptr, end = nullptr, step = nullptr;
			if(it->type == TOKEN_PUNCT_PAREN_CLOSE)
			{
				++it; // Skip ')'
			}
			else
			{
				if(it->type == TOKEN_PUNCT_SEMICOLON)
				{
					++it; // Skip ';'
				}
				else if(it->type == TOKEN_IDENTIFIER || it->type == TOKEN_KEYWORD_VAR)
				{
					start = parseVariableDefinition();
					if(!start)
					{
						// TODO: Handle error
						util::logger->error("Invalid start expression in 'for'");
						return nullptr;
					}

					if(it->type != TOKEN_PUNCT_SEMICOLON)
					{
						// TODO: Handle error
						util::logger->error("Invalid for statement: expected ';' after 'for start', got '{}'", it->value);
						return nullptr;
					}
					++it; // Skip ';'
				}
				else
				{
					// TODO: Handle error
					util::logger->error("Invalid for statement: expected ';' or an identifier in 'for start', got '{}' instead", it->value);
					return nullptr;
				}

				if(it->type == TOKEN_PUNCT_SEMICOLON)
				{
					++it; // Skip ';'
				}
				else
				{
					end = parseExpression();
					if(!end)
					{
						// TODO: Handle error
						util::logger->error("Invalid 'for end' expression");
						return nullptr;
					}

					if(it->type != TOKEN_PUNCT_SEMICOLON)
					{
						// TODO: Handle error
						util::logger->error("Invalid for statement: expected ',' after 'for end', got '{}'", it->value);
						return nullptr;
					}
					++it; // Skip ';'
				}

				if(it->type != TOKEN_PUNCT_PAREN_CLOSE)
				{
					step = parseExpression();
					if(!step)
					{
						// TODO: Handle error
						util::logger->error("Invalid 'for step' expression");
						return nullptr;
					}

					if(it->type != TOKEN_PUNCT_PAREN_CLOSE)
					{
						// TODO: Handle error
						util::logger->error("Invalid for statement: expected ')' after 'for step', got '{}'", it->value);
						return nullptr;
					}
					++it; // Skip ')'
				}
			}

			std::unique_ptr<ASTBlockStatement> block;
			if(it->type == TOKEN_PUNCT_SEMICOLON)
			{
				block = nullptr;
			}
			else if(it->type == TOKEN_PUNCT_BRACE_OPEN)
			{
				block = parseBlockStatement();
				if(!block)
				{
					// TODO: Handle error
					util::logger->error("Invalid for statement block");
					return nullptr;
				}
			}
			else
			{
				auto tmp = parseStatement();
				if(!tmp)
				{
					// TODO: Handle error
					util::logger->error("Invalid for statement");
					return nullptr;
				}
				block->nodes.push_back(std::move(tmp));
			}

			return std::make_unique<ASTForStatement>(std::move(block), std::move(start), std::move(end), std::move(step));
		}

		std::unique_ptr<ASTVariableDefinitionExpression> Parser::parseVariableDefinition()
		{
			if(it->type != TOKEN_IDENTIFIER && it->type != TOKEN_KEYWORD_VAR)
			{
				// TODO: Handle error
				util::logger->error("Invalid variable definition: expected typename or 'var', got '{}' instead", it->value);
				return nullptr;
			}
			std::string typen = it->value;
			bool typeDetermined = it->type == TOKEN_IDENTIFIER;
			++it; // Skip typename/̈́'var'

			if(it->type != TOKEN_IDENTIFIER)
			{
				// TODO: Handle error
				util::logger->error("Invalid variable definition: expected identifier, got '{}' instead", it->value);
				return nullptr;
			}
			std::string name = it->value;
			++it; // Skip variable name

			std::unique_ptr<ASTExpression> init = nullptr;
			if(it->type == TOKEN_PUNCT_SEMICOLON)
			{
				++it; // Skip ';'
			}
			else if(it->type == TOKEN_OPERATORA_SIMPLE)
			{
				++it; // Skip '='
				init = parseExpression();
				if(!init)
				{
					// TODO: Handle error
					util::logger->error("Invalid variable init expression");
					return nullptr;
				}
			}
			else
			{
				// TODO: Handle error
				util::logger->error("Invalid variable definition: expected ';' or assignment operator, got '{}' instead", it->value);
				return nullptr;
			}

			ASTVariableDefinitionExpression::Type type;
			if(!typeDetermined)
			{
				if(!init)
				{
					// TODO: Handle error
					util::logger->error("Invalid variable definition: init expression is required when using 'var'");
					return nullptr;
				}
				else
				{
					// TODO: Determine type of var
					// TODO: Handle error
					util::logger->error("Unimplemented");
					return nullptr;
				}
			}
			else
			{
				type = [this, &typen]()
				{
					if(typen == "Integer") return ASTVariableDefinitionExpression::INTEGER;

					if(typen == "Int8") return ASTVariableDefinitionExpression::INT8;
					if(typen == "Int16") return ASTVariableDefinitionExpression::INT16;
					if(typen == "Int32") return ASTVariableDefinitionExpression::INT32;
					if(typen == "Int64") return ASTVariableDefinitionExpression::INT64;

					if(typen == "UInt8") return ASTVariableDefinitionExpression::UINT8;
					if(typen == "UInt16") return ASTVariableDefinitionExpression::UINT16;
					if(typen == "UInt32") return ASTVariableDefinitionExpression::UINT32;
					if(typen == "UInt64") return ASTVariableDefinitionExpression::UINT64;

					if(typen == "Float") return ASTVariableDefinitionExpression::FLOAT;
					if(typen == "F32") return ASTVariableDefinitionExpression::F32;
					if(typen == "F64") return ASTVariableDefinitionExpression::F64;

					if(typen == "String") return ASTVariableDefinitionExpression::STRING;
					if(typen == "Char") return ASTVariableDefinitionExpression::CHAR;
					if(typen == "Bool") return ASTVariableDefinitionExpression::BOOL;
					if(typen == "None") return ASTVariableDefinitionExpression::NONE;

					return ASTVariableDefinitionExpression::UDEF;
				}();
			}

			auto typename_ = std::make_unique<ASTIdentifierExpression>(typen);
			auto name_ = std::make_unique<ASTIdentifierExpression>(name);
			return std::make_unique<ASTVariableDefinitionExpression>(type, std::move(typename_), std::move(name_), std::move(init));
		}

		std::unique_ptr<ASTExpression> Parser::parsePrimary()
		{
			switch(it->type.get())
			{
			case TOKEN_IDENTIFIER:
				return parseIdentifierExpression();
			case TOKEN_PUNCT_PAREN_OPEN:
				return parseParenExpression();
			case TOKEN_LITERAL_INTEGER:
				return parseIntegerLiteralExpression();
			case TOKEN_LITERAL_FLOAT:
				return parseFloatLiteralExpression();
			case TOKEN_LITERAL_CHAR:
				return parseCharLiteralExpression();
			case TOKEN_LITERAL_STRING:
				return parseStringLiteralExpression();
			case TOKEN_LITERAL_NONE:
				return parseNoneLiteralExpression();
			case TOKEN_LITERAL_FALSE:
				return parseFalseLiteralExpression();
			case TOKEN_LITERAL_TRUE:
				return parseTrueLiteralExpression();
			/*case TOKEN_KEYWORD_IF:
				return parseIfStatement();
			case TOKEN_KEYWORD_FOR:
				return parseForStatement();*/
			case TOKEN_KEYWORD_VAR:
				return parseVariableDefinition();
			}
			// TODO: Handle error
			util::logger->error("Unknown token: '{}'", it->value);
			return nullptr;
		}

		std::unique_ptr<ASTExpression> Parser::parseUnary()
		{
			if(getTokenPrecedence() == -1)
			{
				return parsePrimary();
			}

			TokenVector::const_iterator opc = it;
			++it;
			if(auto operand = parseUnary())
			{
				return std::make_unique<ASTUnaryOperationExpression>(std::move(operand), it->type);
			}
			return nullptr;
		}

		std::unique_ptr<ASTExpression> Parser::parseBinaryOperatorRHS(int prec, std::unique_ptr<ASTExpression> lhs)
		{
			while(it->type != TOKEN_EOF)
			{
				int tokenPrec = getTokenPrecedence();

				if(tokenPrec < prec)
					return lhs;

				TokenVector::const_iterator binop = it;
				++it;

				auto rhs = parseUnary();
				if(!rhs)
				{
					return nullptr;
				}

				int nextPrec = getTokenPrecedence();
				if(tokenPrec < nextPrec)
				{
					rhs = parseBinaryOperatorRHS(tokenPrec + 1, std::move(rhs));
					if(!rhs)
					{
						return nullptr;
					}
				}

				lhs = std::make_unique<ASTBinaryOperationExpression>(std::move(lhs), std::move(rhs), binop->type);
			}
			return nullptr;
		}

		std::unique_ptr<ASTIntegerLiteralExpression> Parser::parseIntegerLiteralExpression()
		{
			const int base = [this]()
			{
				if(it->modifierInt.isSet(INTEGER_HEX)) return 16;
				if(it->modifierInt.isSet(INTEGER_OCTAL)) return 8;
				return 10;
			}();

			try
			{
				if(it->modifierInt.isSet(INTEGER_UNSIGNED))
				{
					if(it->modifierInt.isSet(INTEGER_SHORT))
					{
						auto val = std::stoul(it->value, 0, base);
						if(val > std::numeric_limits<uint16_t>::max())
						{
							throw std::out_of_range(fmt::format("'{}' cannot fit into UInt16", it->value));
						}
						return std::make_unique<ASTIntegerLiteralExpression>(static_cast<uint16_t>(val), it->modifierInt);
					}
					else if(it->modifierInt.isSet(INTEGER_LONG))
					{
						auto val = std::stoull(it->value, 0, base);
						return std::make_unique<ASTIntegerLiteralExpression>(static_cast<uint64_t>(val), it->modifierInt);
					}
					else
					{
						auto val = std::stoul(it->value, 0, base);
						return std::make_unique<ASTIntegerLiteralExpression>(static_cast<uint32_t>(val), it->modifierInt);
					}
				}
				else
				{
					if(it->modifierInt.isSet(INTEGER_SHORT))
					{
						auto val = std::stol(it->value, 0, base);
						if(val > std::numeric_limits<int16_t>::max() || val < std::numeric_limits<int16_t>::min())
						{
							throw std::out_of_range(fmt::format("'{}' cannot fit into Int16", it->value));
						}
						return std::make_unique<ASTIntegerLiteralExpression>(static_cast<int16_t>(val), it->modifierInt);
					}
					else if(it->modifierInt.isSet(INTEGER_LONG))
					{
						auto val = std::stoll(it->value, 0, base);
						return std::make_unique<ASTIntegerLiteralExpression>(static_cast<int64_t>(val), it->modifierInt);
					}
					else
					{
						auto val = std::stol(it->value, 0, base);
						return std::make_unique<ASTIntegerLiteralExpression>(static_cast<int32_t>(val), it->modifierInt);
					}
				}
			}
			catch(std::invalid_argument &e)
			{
				// TODO: Handle error
				util::logger->error("Invalid integer literal: literal is ill-formed: '{}'. Description: '{}'", it->value, e.what());
				return nullptr;
			}
			catch(std::out_of_range &e)
			{
				// TODO: Handle error
				util::logger->error("Invalid integer literal: value out of range: '{}'. Description: '{}'", it->value, e.what());
				return nullptr;
			}
			return nullptr;
		}
		std::unique_ptr<ASTFloatLiteralExpression> Parser::parseFloatLiteralExpression()
		{
			try
			{
				if(it->modifierFloat.isSet(FLOAT_FLOAT))
				{
					auto val = std::stof(it->value);
					return std::make_unique<ASTFloatLiteralExpression>(static_cast<float>(val), it->modifierFloat);
				}
				else
				{
					auto val = std::stod(it->value);
					return std::make_unique<ASTFloatLiteralExpression>(static_cast<double>(val), it->modifierFloat);
				}
			}
			catch(std::invalid_argument &e)
			{
				// TODO: Handle error
				util::logger->error("Invalid float literal: literal is ill-formed: '{}'. Description: '{}'", it->value, e.what());
				return nullptr;
			}
			catch(std::out_of_range &e)
			{
				// TODO: Handle error
				util::logger->error("Invalid float literal: value out of range: '{}'. Description: '{}'", it->value, e.what());
				return nullptr;
			}
			return nullptr;
		}


		std::unique_ptr<ASTStringLiteralExpression> Parser::parseStringLiteralExpression()
		{
			return std::make_unique<ASTStringLiteralExpression>(it->value);
		}
		std::unique_ptr<ASTCharLiteralExpression> Parser::parseCharLiteralExpression()
		{
			try
			{
				auto val = std::stol(it->value);
				return std::make_unique<ASTCharLiteralExpression>(static_cast<char32_t>(val));
			}
			catch(std::invalid_argument &e)
			{
				// TODO: Handle error
				util::logger->error("Invalid char literal: literal is ill-formed: '{}'. Description: '{}'", it->value, e.what());
				return nullptr;
			}
			catch(std::out_of_range &e)
			{
				// TODO: Handle error
				util::logger->error("Invalid char literal: value out of range: '{}'. Description: '{}'", it->value, e.what());
				return nullptr;
			}
			return nullptr;
		}
		std::unique_ptr<ASTBoolLiteralExpression> Parser::parseTrueLiteralExpression()
		{
			return std::make_unique<ASTBoolLiteralExpression>(true);
		}
		std::unique_ptr<ASTBoolLiteralExpression> Parser::parseFalseLiteralExpression()
		{
			return std::make_unique<ASTBoolLiteralExpression>(false);
		}
		std::unique_ptr<ASTNoneLiteralExpression> Parser::parseNoneLiteralExpression()
		{
			return std::make_unique<ASTNoneLiteralExpression>();
		}

		std::unique_ptr<ASTExpression> Parser::parseExpression()
		{
			auto lhs = parseUnary();
			if(!lhs)
			{
				return nullptr;
			}
			return parseBinaryOperatorRHS(0, std::move(lhs));
		}
	}
}
