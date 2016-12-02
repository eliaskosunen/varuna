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
#include "core/ast/ASTFunctionStatement.h"
#include "core/ast/ASTControlStatement.h"
#include "core/ast/ASTOperatorExpression.h"
#include "core/ast/ASTLiteralExpression.h"
#include "core/ast/DumpASTVisitor.h"
#include "core/lexer/Token.h"
#include "util/Compatibility.h"
#include "util/StringUtils.h"

#include <iterator> // For std::next
#include <limits>

// TODO: Needs cleanups and refactoring

namespace core
{
	namespace parser
	{
		using namespace core::lexer;
		using namespace core::ast;

		void Parser::run()
		{
			_runParser();
		}

		void Parser::_runParser()
		{
			while(true)
			{
				// If reached end of token list, throw
				// This should not be possible, EOF should be reached first
				if(it == endTokens)
				{
					throw std::logic_error("EOF not in the end of the token list");
				}

				// Check current token
				switch(it->type.get())
				{
				// In case of EOF, stop
				case TOKEN_EOF:
					return;

				// A lonely semicolon top-level token
				case TOKEN_PUNCT_SEMICOLON:
					// This means an empty statement
					// That's a warning
					parserWarning("Empty statement");
					handleEmptyStatement(); // Skips the ';'
					break;

				// Import statement
				case TOKEN_KEYWORD_IMPORT:
					handleImport();
					break;
				// Module statement
				case TOKEN_KEYWORD_MODULE:
					handleModule();
					break;
				// Function definition
				case TOKEN_KEYWORD_DEFINE:
					handleDef();
					break;

				// Unsupported top-level token
				default:
					parserError("'{}' is not allowed as a top-level token", it->value);
					return;
				}
			}
		}

		std::unique_ptr<ASTImportStatement> Parser::parseImportStatement()
		{
			// Import statement syntax:
			// "import" [package/module] identifier/literal-string ;

			++it; // Skip "import"

			// Parse import type
			// Options:
			// 	* module
			// 	* package
			// Not required
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

			// Parse module/file to import
			// Either an identifier or a string literal
			std::string toImport;
			bool isPath = false;

			// Identifier:
			// Import a module/package by module/package statement
			if(it->type == TOKEN_IDENTIFIER)
			{
				// '.' is a operator, construct string of module/package to import
				while(true)
				{
					toImport += it->value;

					// EOF or ';'
					// Importee done
					if(std::next(it)->type == TOKEN_EOF ||
						std::next(it)->type == TOKEN_PUNCT_SEMICOLON)
					{
						break;
					}

					// '.' or identifier
					// Add to toImport
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
			// String literal:
			// Import a module/package by file path
			else if(it->type == TOKEN_LITERAL_STRING)
			{
				isPath = true;
				toImport = it->value;
			}
			else
			{
				return parserError("Invalid importee: '{}'", it->value);
			}

			// Semicolon required after import statement
			if(std::next(it)->type != TOKEN_PUNCT_SEMICOLON)
			{
				return parserError("Expected semicolon after import statement");
			}
			++it; // Skip semicolon

			auto toImportObj = std::make_unique<ASTIdentifierExpression>(toImport);
			auto stmt = std::make_unique<ASTImportStatement>(importType, std::move(toImportObj), isPath);
			++it;
			return stmt;
		}

		std::unique_ptr<ASTIfStatement> Parser::parseIfStatement()
		{
			// If statement syntax:
			// "if" ( [expression] ) statement [ else statement ]

			++it; // Skip 'if'

			// Parse condition
			if(it->type != TOKEN_PUNCT_PAREN_OPEN)
			{
				return parserError("Invalid if statement: Expected '(', got '{}' instead", it->value);
			}
			++it; // Skip '('
			auto cond = [this]() -> std::unique_ptr<ASTExpression>
			{
				// No condition set
				if(it->type == TOKEN_PUNCT_PAREN_CLOSE)
				{
					// If-condition is true by default
					return std::make_unique<ASTBoolLiteralExpression>(true);
				}
				return parseExpression();
			}();
			if(!cond)
			{
				return nullptr;
			}
			if(it->type != TOKEN_PUNCT_PAREN_CLOSE)
			{
				return parserError("Invalid if statement: Expected ')', got '{}' instead", it->value);
			}
			++it; // Skip ')'

			// Parse then statement
			auto then = parseStatement();
			if(!then)
			{
				return nullptr;
			}

			// Parse else statement
			// Optional
			auto elsestmt = [this]() -> std::unique_ptr<ASTStatement>
			{
				if(it->type != TOKEN_KEYWORD_ELSE) return std::make_unique<ASTEmptyStatement>();

				++it; // Skip else
				return parseStatement();
			}();
			if(!elsestmt)
			{
				return nullptr;
			}

			return std::make_unique<ASTIfStatement>(std::move(cond), std::move(then), std::move(elsestmt));
		}

		std::tuple<std::unique_ptr<ASTExpression>, std::unique_ptr<ASTExpression>, std::unique_ptr<ASTExpression>>
			Parser::parseForCondition()
		{
			// Parse the condition of a for statement
			// ( [init] ; [end] ; [step] )

			auto makeTuple = [](std::unique_ptr<ASTExpression> i, std::unique_ptr<ASTExpression> e, std::unique_ptr<ASTExpression> s)
				-> std::tuple<std::unique_ptr<ASTExpression>, std::unique_ptr<ASTExpression>, std::unique_ptr<ASTExpression>>
			{
				return std::make_tuple<std::unique_ptr<ASTExpression>, std::unique_ptr<ASTExpression>, std::unique_ptr<ASTExpression>>(std::move(i), std::move(e), std::move(s));
			};
			auto errorToTuple = [this, makeTuple](std::nullptr_t e)
				-> auto
			{
				return makeTuple(e, e, e);
			};

			if(it->type != TOKEN_PUNCT_PAREN_OPEN)
			{
				return errorToTuple(parserError("Invalid for statement: expected '(' after 'for', got '{}'", it->value));
			}
			++it; // Skip '('

			// Empty for condition
			if(it->type == TOKEN_PUNCT_PAREN_CLOSE)
			{
				++it; // Skip ')'
				// Default: ( empty ; "true" ; empty )
				return makeTuple(std::make_unique<ASTEmptyExpression>(), std::make_unique<ASTBoolLiteralExpression>(true), std::make_unique<ASTEmptyExpression>());
			}

			// Parse init expression
			// Must be either a variable definition or empty
			auto init = [this]() -> std::unique_ptr<ASTExpression>
			{
				if(it->type == TOKEN_PUNCT_SEMICOLON)
				{
					return std::make_unique<ASTEmptyExpression>();
				}

				return parseVariableDefinition();
			}();
			if(!init)
			{
				return errorToTuple(parserError("Invalid for init expression"));
			}
			if(it->type != TOKEN_PUNCT_SEMICOLON)
			{
				return errorToTuple(parserError("Invalid for statement: expected ';' after 'for init', got '{}'", it->value));
			}
			++it; // Skip ';'

			// Parse end expression
			// Must be either an expression or empty
			auto end = [this]() -> std::unique_ptr<ASTExpression>
			{
				if(it->type == TOKEN_PUNCT_SEMICOLON)
				{
					// "true" by default
					return std::make_unique<ASTBoolLiteralExpression>(true);
				}

				return parseExpression();
			}();
			if(!end)
			{
				return errorToTuple(parserError("Invalid for end expression"));
			}
			if(it->type != TOKEN_PUNCT_SEMICOLON)
			{
				return errorToTuple(parserError("Invalid for statement: expected ';' after 'for end', got '{}'", it->value));
			}
			++it; // Skip ';'

			// Parse step expression
			// Must be either an expression or empty
			auto step = [this]() -> std::unique_ptr<ASTExpression>
			{
				if(it->type == TOKEN_PUNCT_PAREN_CLOSE)
				{
					return std::make_unique<ASTEmptyExpression>();
				}

				return parseExpression();
			}();
			if(!step)
			{
				return errorToTuple(parserError("Invalid for step expression"));
			}
			if(it->type != TOKEN_PUNCT_PAREN_CLOSE)
			{
				return errorToTuple(parserError("Invalid for statement: expected ')' after 'for step', got '{}'", it->value));
			}
			++it; // Skip ')'

			return makeTuple(std::move(init), std::move(end), std::move(step));
		}

		std::unique_ptr<ASTForStatement> Parser::parseForStatement()
		{
			// For-statement syntax
			// "for" () statement
			// "for" ( [expression] ; [expression] ; [expression] ) statement

			++it; // Skip 'for'

			// TODO: Optimize
			// Not utilizing constructors
			// Not sure if possible in C++14
			std::unique_ptr<ASTExpression> init, end, step;
			std::tie(init, end, step) = parseForCondition();

			// Parse statement
			std::unique_ptr<ASTStatement> block = parseStatement();
			if(!block)
			{
				return nullptr;
			}

			return std::make_unique<ASTForStatement>(std::move(block), std::move(init), std::move(end), std::move(step));
		}

		std::unique_ptr<ASTVariableDefinitionExpression> Parser::parseVariableDefinition()
		{
			// Variable definition syntax:
			// var/identifier name [ = init-expression ] ;

			// Parse typename
			if(it->type != TOKEN_IDENTIFIER && it->type != TOKEN_KEYWORD_VAR)
			{
				return parserError("Invalid variable definition: expected typename or 'var', got '{}' instead", it->value);
			}
			std::string typen = it->value;
			bool typeDetermined = it->type == TOKEN_IDENTIFIER; // Whether or not the type will have to
																// be determined by the type of the init expression
			++it; // Skip typename/'var'

			// Check if it's an array
			uint32_t arraySize = 0;
			if(it->type == TOKEN_PUNCT_SQR_OPEN) // It's an array
			{
				++it; // Skip '['
				// Array size is required
				auto size = parseIntegerLiteralExpression();
				if(!size)
				{
					return parserError("Invalid array size");
				}

				util::logger->trace("It's an array: {}", size->value);
				if(size->value < 0)
				{
					return parserError("Invalid array size: Array size cannot be negative, got {}", size->value);
				}
				if(size->value == 0)
				{
					return parserError("Invalid array size: Array size cannot be 0");
				}
				if(size->value > std::numeric_limits<int32_t>::max())
				{
					return parserError("Invalid array size: Array size cannot exceed {}, got {}", std::numeric_limits<int32_t>::max(), size->value);
				}
				arraySize = static_cast<uint32_t>(size->value);

				// ']' required
				if(it->type != TOKEN_PUNCT_SQR_CLOSE)
				{
					return parserError("Invalid array definition: Expected ']', got '{}' instead", it->value);
				}
				++it; // Skip ']'
			}

			// Parse variable name
			if(it->type != TOKEN_IDENTIFIER)
			{
				return parserError("Invalid variable definition: expected identifier, got '{}' instead", it->value);
			}
			std::string name = it->value;
			++it; // Skip variable name

			// Parse possible init expression
			auto init = [this]() -> std::unique_ptr<ASTExpression>
			{
				if(it->type == TOKEN_OPERATORA_SIMPLE)
				{
					++it; // Skip '='
					return parseExpression();
				}

				// No init expression
				return std::make_unique<ASTEmptyExpression>();
			}();
			if(!init)
			{
				return parserError("Invalid variable init expression");
			}

			// Determine the type of the variable
			ASTVariableDefinitionExpression::Type type;
			if(!typeDetermined)
			{
				if(!init)
				{
					return parserError("Invalid variable definition: init expression is required when using 'var'");
				}
				else
				{
					// TODO: Determine type of var
					return parserError("Unimplemented");
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
			return std::make_unique<ASTVariableDefinitionExpression>(type, std::move(typename_), std::move(name_), std::move(init), arraySize);
		}

		std::unique_ptr<ASTModuleStatement> Parser::parseModuleStatement()
		{
			// Module statement syntax
			// "module" identifier ;

			++it; // Skip 'module'

			if(it->type != TOKEN_IDENTIFIER)
			{
				return parserError("Invalid module statement: expected identifier after 'module', got '{}' instead", it->value);
			}

			// Construct module name
			std::string name;
			while(true)
			{
				if(it->type == TOKEN_IDENTIFIER)
				{
					name += it->value;
				}
				else
				{
					return parserError("Invalid module statement: expected identifier, got '{}' instead", it->value);
				}
				++it;

				if(it->type == TOKEN_OPERATORB_MEMBER)
				{
					name += it->value;
					if(std::next(it)->type == TOKEN_EOF || std::next(it)->type == TOKEN_PUNCT_SEMICOLON)
					{
						return parserError("Invalid module statement: expected identifier after '.', got '{}' instead", it->value);
					}
				}
				else if(it->type == TOKEN_PUNCT_SEMICOLON)
				{
					++it; // Skip ';'
					break;
				}
				else
				{
					return parserError("Invalid module statement: expected '.' or ';', got '{}' instead", it->value);
				}
				++it; // Skip '.'
			}

			auto moduleName = std::make_unique<ASTIdentifierExpression>(name);
			return std::make_unique<ASTModuleStatement>(std::move(moduleName));
		}

		std::unique_ptr<ASTExpression> Parser::parseParenExpression()
		{
			// Parse an expression in parenthesis
			++it; // Skip '('
			auto v = parseExpression();
			if(!v)
			{
				return nullptr;
			}

			if(it->type != TOKEN_PUNCT_PAREN_CLOSE)
			{
				return parserError("Expected closing parenthesis, got '{}' instead", it->value);
			}
			++it; // Skip ')'
			return v;
		}

		std::unique_ptr<ASTExpression> Parser::parseIdentifierExpression()
		{
			std::string idName = it->value;
			++it; // Skip identifier

			// If there is an identifier or a '[' (meaning array) after the initial identifier,
			// it's a variable definition
			if(it->type == TOKEN_IDENTIFIER || it->type == TOKEN_PUNCT_SQR_OPEN)
			{
				--it; // Roll back to the previous identifier, which is the typename
				return parseVariableDefinition();
			}

			// Member variable/function
			if(it->type == TOKEN_OPERATORB_MEMBER)
			{
				while(true)
				{
					if(it->type == TOKEN_EOF)
					{
						return parserError("Invalid member access operand: '{}'", it->value);
					}
					if(it->type != TOKEN_OPERATORB_MEMBER)
					{
						break;
					}
					idName += it->value;
					++it; // Skip '.'
					if(it->type != TOKEN_IDENTIFIER)
					{
						return parserError("Invalid member access operand: Expected identifier, got '{}' instead", it->value);
					}
					idName += it->value;
					++it; // Skip identifier
				}
			}

			// Function call
			if(it->type == TOKEN_PUNCT_PAREN_OPEN)
			{
				++it; // Skip '('
				std::vector<std::unique_ptr<ASTExpression>> args;
				if(it->type != TOKEN_PUNCT_PAREN_CLOSE)
				{
					// Parse argument list
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
							return parserError("Invalid function call: Expected ')' or ',' in argument list, got '{}' instead", it->value);
						}
						++it; // Skip ','
					}
				}

				++it; // Skip ')'
				auto id = std::make_unique<ASTIdentifierExpression>(idName);
				return std::make_unique<ASTCallExpression>(std::move(id), std::move(args));
			}

			return std::make_unique<ASTVariableRefExpression>(idName);
		}

		std::unique_ptr<ASTExpression> Parser::parsePrimary()
		{
			// Parse primary expression
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
			case TOKEN_KEYWORD_VAR:
				return parseVariableDefinition();
			default:
				return parserError("Unknown token: '{}'", it->value);
			}
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
				return std::make_unique<ASTUnaryOperationExpression>(std::move(operand), opc->type);
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

			const TokenVector::const_iterator lit = it;
			++it;

			try
			{
				auto size = [lit]() -> decltype(lit->modifierInt.get())
				{
					if(lit->modifierInt.isSet(INTEGER_LONG)) return INTEGER_LONG;
					if(lit->modifierInt.isSet(INTEGER_SHORT)) return INTEGER_SHORT;
					return INTEGER_INTEGER;
				}();

				int64_t val = std::stoll(lit->value, 0, base);
				if(size == INTEGER_SHORT)
				{
					if(val > std::numeric_limits<int16_t>::max() || val < std::numeric_limits<int16_t>::min())
					{
						throw std::out_of_range(fmt::format("'{}' cannot fit into Int16", lit->value));
					}
				}
				else if(size == INTEGER_INTEGER)
				{
					if(val > std::numeric_limits<int32_t>::max() || val < std::numeric_limits<int32_t>::min())
					{
						throw std::out_of_range(fmt::format("'{}' cannot fit into Int32", lit->value));
					}
				}
				return std::make_unique<ASTIntegerLiteralExpression>(val, lit->modifierInt);

			}
			catch(std::invalid_argument &e)
			{
				return parserError("Invalid integer literal: literal is ill-formed: '{}'. Description: '{}'", lit->value, e.what());
			}
			catch(std::out_of_range &e)
			{
				return parserError("Invalid integer literal: value out of range: '{}'. Description: '{}'", lit->value, e.what());
			}
		}
		std::unique_ptr<ASTFloatLiteralExpression> Parser::parseFloatLiteralExpression()
		{
			const TokenVector::const_iterator lit = it;
			++it;

			try
			{
				if(lit->modifierFloat.isSet(FLOAT_FLOAT))
				{
					float val = std::stof(lit->value);
					return std::make_unique<ASTFloatLiteralExpression>(val, lit->modifierFloat);
				}
				else
				{
					double val = std::stod(lit->value);
					return std::make_unique<ASTFloatLiteralExpression>(val, lit->modifierFloat);
				}
			}
			catch(std::invalid_argument &e)
			{
				return parserError("Invalid float literal: literal is ill-formed: '{}'. Description: '{}'", lit->value, e.what());
			}
			catch(std::out_of_range &e)
			{
				return parserError("Invalid float literal: value out of range: '{}'. Description: '{}'", lit->value, e.what());
			}
		}


		std::unique_ptr<ASTStringLiteralExpression> Parser::parseStringLiteralExpression()
		{
			const TokenVector::const_iterator lit = it;
			++it;

			return std::make_unique<ASTStringLiteralExpression>(lit->value);
		}
		std::unique_ptr<ASTCharLiteralExpression> Parser::parseCharLiteralExpression()
		{
			const TokenVector::const_iterator lit = it;
			++it;

			try
			{
				if(lit->value.length() > 1)
				{
					throw std::out_of_range("Char literal length more than 1");
				}
				auto val = lit->value.at(0);
				return std::make_unique<ASTCharLiteralExpression>(static_cast<char32_t>(val));
			}
			catch(std::out_of_range &e)
			{
				return parserError("Invalid char literal: value out of range: '{}'. Description: '{}'", lit->value, e.what());
			}
		}
		std::unique_ptr<ASTBoolLiteralExpression> Parser::parseTrueLiteralExpression()
		{
			++it;
			return std::make_unique<ASTBoolLiteralExpression>(true);
		}
		std::unique_ptr<ASTBoolLiteralExpression> Parser::parseFalseLiteralExpression()
		{
			++it;
			return std::make_unique<ASTBoolLiteralExpression>(false);
		}
		std::unique_ptr<ASTNoneLiteralExpression> Parser::parseNoneLiteralExpression()
		{
			++it;
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

		void Parser::handleImport()
		{
			if(auto import = parseImportStatement())
			{
				util::logger->trace("Parsed import statement: importee: '{}', isPath: '{}', type: '{}'",
					import->importee->value, import->isPath, static_cast<int>(import->importType));
				getAST()->pushStatement(std::move(import));
			}
			else
			{
				++it;
			}
		}

		void Parser::handleModule()
		{
			if(auto module = parseModuleStatement())
			{
				util::logger->trace("Parsed module statement: moduleName: '{}'",
					module->moduleName->value);
				getAST()->pushStatement(std::move(module));
			}
			else
			{
				++it;
			}
		}

		void Parser::handleDef()
		{
			if(auto def = parseFunctionDefinitionStatement())
			{
				util::logger->trace("Parsed function definition: name: '{}', return: '{}', params.size: '{}'", def->proto->name->value, def->proto->returnType->value, def->proto->params.size());
				getAST()->pushStatement(std::move(def));
			}
			else
			{
				++it;
			}
		}

		void Parser::handleEmptyStatement()
		{
			if(auto stmt = emptyStatement())
			{
				util::logger->trace("Parsed empty statement");
				getAST()->pushStatement(std::move(stmt));
			}
			else
			{
				++it;
			}
		}

		std::unique_ptr<ASTStatement> Parser::parseStatement()
		{
			switch(it->type.get())
			{
			case TOKEN_KEYWORD_IF:
				return parseIfStatement();
			case TOKEN_KEYWORD_FOR:
				return parseForStatement();

			case TOKEN_KEYWORD_VAR:
			case TOKEN_IDENTIFIER:
			{
				auto expr = parseIdentifierExpression();
				if(!expr)
				{
					return nullptr;
				}
				auto stmt = wrapExpression(std::move(expr));
				if(!stmt)
				{
					return nullptr;
				}
				return std::move(stmt);
			}
			case TOKEN_PUNCT_BRACE_OPEN:
				return parseBlockStatement();
			case TOKEN_PUNCT_SEMICOLON:
				parserWarning("Empty statement");
				return emptyStatement();
			default:
				return parserError("Unknown statement: '{}'", it->value);
			}
		}

		std::unique_ptr<ASTBlockStatement> Parser::parseBlockStatement()
		{
			++it; // Skip '{'
			auto block = std::make_unique<ASTBlockStatement>();
			while(true)
			{
				if(it->type == TOKEN_EOF)
				{
					return parserError("Unexpected token: '{}'", it->value);
				}
				if(it->type == TOKEN_PUNCT_BRACE_CLOSE)
				{
					++it; // Skip '}'
					break;
				}

				auto stmt = parseStatement();
				if(!stmt)
				{
					return nullptr;
				}
				block->nodes.push_back(std::move(stmt));
			}
			return block;
		}

		std::unique_ptr<ASTFunctionPrototypeStatement> Parser::parseFunctionPrototype()
		{
			if(it->type != TOKEN_IDENTIFIER)
			{
				return parserError("Invalid function prototype: expected identifier, got '{}' instead", it->value);
			}
			auto funcName = std::make_unique<ASTIdentifierExpression>(it->value);
			++it; // Skip identifier

			if(it->type != TOKEN_PUNCT_PAREN_OPEN)
			{
				return parserError("Invalid function prototype: expected '(', got '{}' instead", it->value);
			}
			++it; // Skip '('

			std::vector<std::unique_ptr<ASTFunctionParameter>> params;
			while(true)
			{
				if(it->type == TOKEN_PUNCT_PAREN_CLOSE)
				{
					++it; // Skip ')'
					break;
				}

				auto type = [this]()
				{
					if(it->type == TOKEN_KEYWORD_REF)
					{
						++it;
						return ASTFunctionParameter::REF;
					}
					if(it->type == TOKEN_KEYWORD_VIEW)
					{
						++it;
						return ASTFunctionParameter::VIEW;
					}
					return ASTFunctionParameter::COPY;
				}();

				auto var = parseVariableDefinition();
				if(!var)
				{
					return parserError("Invalid variable definition in function prototype");
				}

				auto param = std::make_unique<ASTFunctionParameter>(std::move(var), type);
				params.push_back(std::move(param));

				if(it->type == TOKEN_EOF)
				{
					return parserError("Invalid function prototype: expected ')' or ',' in parameter list, got '{}' instead", it->value);
				}
				else if(it->type == TOKEN_PUNCT_COMMA)
				{
					++it; // Skip ','
					continue;
				}
				else if(it->type == TOKEN_PUNCT_PAREN_CLOSE)
				{
					++it; // Skip ')'
					break;
				}
				else
				{
					return parserError("Invalid function prototype: expected ')' or ',' in parameter, got '{}' instead", it->value);
				}
			}

			if(it->type != TOKEN_PUNCT_COLON)
			{
				return parserError("Invalid function prototype: expected ':' before return type, got '{}' instead", it->value);
			}
			++it; // Skip ':'

			if(it->type != TOKEN_IDENTIFIER)
			{
				return parserError("Invalid function prototype: expected identifier in return type, got '{}' instead", it->value);
			}
			auto returnType = std::make_unique<ASTIdentifierExpression>(it->value);
			++it; // Skip identifier

			return std::make_unique<ASTFunctionPrototypeStatement>(std::move(funcName), std::move(returnType), std::move(params));
		}

		std::unique_ptr<ASTFunctionDefinitionStatement> Parser::parseFunctionDefinitionStatement()
		{
			++it; // Skip 'def'
			auto proto = parseFunctionPrototype();
			if(!proto)
			{
				return parserError("Invalid function definition");
			}

			if(it->type != TOKEN_PUNCT_BRACE_OPEN)
			{
				return parserError("Invalid function definition: expected '{' to start function body, got '{}' instead", it->value);
			}
			if(auto body = parseBlockStatement())
			{
				return std::make_unique<ASTFunctionDefinitionStatement>(std::move(proto), std::move(body));
			}
			return parserError("Invalid function definition body");
		}

		std::unique_ptr<ASTWrappedExpressionStatement> Parser::wrapExpression(std::unique_ptr<ASTExpression> expr)
		{
			if(!expr)
			{
				return parserError("Trying to wrap a nullptr expression");
			}
			if(it->type != TOKEN_PUNCT_SEMICOLON)
			{
				return parserError("Expected semicolon after expression statement, got '{}' instead", it->value);
			}
			++it;
			return std::make_unique<ASTWrappedExpressionStatement>(std::move(expr));
		}

		int Parser::getTokenPrecedence() const
		{
			using namespace core::lexer;
			switch(it->type.get())
			{
			case TOKEN_OPERATORA_SIMPLE:
			case TOKEN_OPERATORA_ADD:
			case TOKEN_OPERATORA_SUB:
			case TOKEN_OPERATORA_MUL:
			case TOKEN_OPERATORA_DIV:
			case TOKEN_OPERATORA_MOD:
				return 10;

			case TOKEN_OPERATORB_OR:
				return 20;
			case TOKEN_OPERATORB_AND:
				return 30;

			case TOKEN_OPERATORB_EQ:
			case TOKEN_OPERATORB_NOTEQ:
				return 70;
			case TOKEN_OPERATORB_GREATER:
			case TOKEN_OPERATORB_GREATEQ:
			case TOKEN_OPERATORB_LESS:
			case TOKEN_OPERATORB_LESSEQ:
				return 80;

			case TOKEN_OPERATORB_ADD:
			case TOKEN_OPERATORB_SUB:
				return 90;
			case TOKEN_OPERATORB_MUL:
			case TOKEN_OPERATORB_DIV:
			case TOKEN_OPERATORB_MOD:
				return 100;

			case TOKEN_OPERATORU_NOT:
			case TOKEN_OPERATORU_PLUS:
			case TOKEN_OPERATORU_MINUS:
			case TOKEN_OPERATORU_SIZEOF:
			case TOKEN_OPERATORU_TYPEOF:
			case TOKEN_OPERATORU_INSTOF:
			case TOKEN_OPERATORU_ADDROF:
			case TOKEN_OPERATORU_DEC:
			case TOKEN_OPERATORU_INC:
				return 110;

			case TOKEN_OPERATORB_MEMBER:
				return 120;

			default:
				return -1;
			}
		}
	}
}
