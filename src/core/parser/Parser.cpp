// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#include "core/parser/Parser.h"
#include "core/ast/ASTControlStatement.h"
#include "core/ast/ASTExpression.h"
#include "core/ast/ASTFunctionStatement.h"
#include "core/ast/ASTLiteralExpression.h"
#include "core/ast/ASTOperatorExpression.h"
#include "core/ast/ASTParentSolverVisitor.h"
#include "core/ast/DumpASTVisitor.h"
#include "core/lexer/Token.h"
#include "util/Compatibility.h"
#include "util/StringUtils.h"
#include <iterator> // For std::next
#include <limits>
#include <stack>

namespace core
{
namespace parser
{
    using namespace core::lexer;
    using namespace core::ast;

    Parser::Parser(const core::lexer::TokenVector& tok)
        : warningsAsErrors(false),
          ast(std::make_unique<ast::AST>(tok.front().loc.file)), tokens(tok),
          it(tokens.begin()), endTokens(tokens.end()), error(ERROR_NONE)
    {
    }

    void Parser::run()
    {
        _runParser();

        auto parentSolver = std::make_unique<ASTParentSolverVisitor>();
        parentSolver->run<ASTBlockStatement>(ast->globalNode.get());
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
            // Global variable
            case TOKEN_KEYWORD_LET:
            case TOKEN_KEYWORD_VAR:
                handleGlobalVariable();
                break;
            // Exported symbol
            case TOKEN_KEYWORD_EXPORT:
                handleExport();
                break;

            // Unsupported top-level token
            default:
                parserError("'{}' is not allowed as a top-level token",
                            it->value);
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
        ASTImportStatement::ImportType importType =
            ASTImportStatement::UNSPECIFIED;
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
                util::logger->error("Invalid importee: '{}{}'", toImport,
                                    it->value);
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
        auto stmt = std::make_unique<ASTImportStatement>(
            importType, std::move(toImportObj), isPath);
        ++it;
        return stmt;
    }

    std::unique_ptr<ASTIfStatement> Parser::parseIfStatement()
    {
        // If statement syntax:
        // "if" ( [expression] ) statement [ else statement ]

        ++it; // Skip 'if'

        // Parse condition
        if(it->type == TOKEN_PUNCT_PAREN_OPEN)
        {
            ++it; // Skip '('
        }
        auto cond = [&]() -> std::unique_ptr<ASTExpression> {
            // No condition set
            if(it->type == TOKEN_PUNCT_PAREN_CLOSE ||
               it->type == TOKEN_PUNCT_SEMICOLON ||
               it->type == TOKEN_PUNCT_BRACE_OPEN)
            {
                return parserError("No if condition given");
            }
            return parseExpression();
        }();
        if(!cond)
        {
            return nullptr;
        }
        if(it->type == TOKEN_PUNCT_PAREN_CLOSE)
        {
            ++it; // Skip ')'
        }

        // Parse then statement
        auto then = parseStatement();
        if(!then)
        {
            return nullptr;
        }

        // Parse else statement
        // Optional
        auto elsestmt = [&]() -> std::unique_ptr<ASTStatement> {
            if(it->type != TOKEN_KEYWORD_ELSE)
            {
                return std::make_unique<ASTEmptyStatement>();
            }

            ++it; // Skip else
            return parseStatement();
        }();
        if(!elsestmt)
        {
            return nullptr;
        }

        return std::make_unique<ASTIfStatement>(
            std::move(cond), std::move(then), std::move(elsestmt));
    }

    std::unique_ptr<ASTWhileStatement> Parser::parseWhileStatement()
    {
        // While statement syntax:
        // "while" ( [expression] ) statement

        ++it; // Skip 'while'

        // Parse condition
        if(it->type == TOKEN_PUNCT_PAREN_OPEN)
        {
            ++it; // Skip '('
        }
        auto cond = [&]() -> std::unique_ptr<ASTExpression> {
            // No condition set
            if(it->type == TOKEN_PUNCT_PAREN_CLOSE ||
               it->type == TOKEN_PUNCT_SEMICOLON ||
               it->type == TOKEN_PUNCT_BRACE_OPEN)
            {
                return parserError("No while condition given");
            }
            return parseExpression();
        }();
        if(!cond)
        {
            return nullptr;
        }
        if(it->type == TOKEN_PUNCT_PAREN_CLOSE)
        {
            ++it; // Skip ')'
        }

        auto body = parseStatement();
        if(!body)
        {
            return nullptr;
        }

        return std::make_unique<ASTWhileStatement>(std::move(cond),
                                                   std::move(body));
    }

    std::tuple<std::unique_ptr<ASTExpression>, std::unique_ptr<ASTExpression>,
               std::unique_ptr<ASTExpression>>
    Parser::parseForCondition()
    {
        // Parse the condition of a for statement
        // [ ( ] [init] , [end] , [step] [ ) ]

        auto makeTuple = [](std::unique_ptr<ASTExpression> i,
                            std::unique_ptr<ASTExpression> e,
                            std::unique_ptr<ASTExpression> s) {
            return std::make_tuple(std::move(i), std::move(e), std::move(s));
        };
        auto errorToTuple = [&](std::nullptr_t e) -> auto
        {
            return makeTuple(e, e, e);
        };

        if(it->type == TOKEN_PUNCT_PAREN_OPEN)
        {
            ++it; // Skip '('
        }

        // Empty for condition
        if(it->type == TOKEN_PUNCT_PAREN_CLOSE ||
           it->type == TOKEN_PUNCT_BRACE_OPEN ||
           it->type == TOKEN_PUNCT_SEMICOLON)
        {
            ++it; // Skip ')' / '{' / ';'
            return errorToTuple(parserError("Empty for condition"));
        }

        // Parse init expression
        // Must be either a variable definition or empty
        auto init = [&]() -> std::unique_ptr<ASTExpression> {
            if(it->type == TOKEN_PUNCT_COMMA)
            {
                return std::make_unique<ASTEmptyExpression>();
            }

            return parseVariableDefinition();
        }();
        if(!init)
        {
            return errorToTuple(parserError("Invalid for init expression"));
        }
        if(it->type != TOKEN_PUNCT_COMMA)
        {
            return errorToTuple(parserError("Invalid for statement: expected "
                                            "',' after 'for init', got '{}'",
                                            it->value));
        }
        ++it; // Skip ','

        // Parse end expression
        // Must be either an expression or empty
        auto end = [&]() -> std::unique_ptr<ASTExpression> {
            if(it->type == TOKEN_PUNCT_COMMA)
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
        if(it->type != TOKEN_PUNCT_COMMA)
        {
            return errorToTuple(parserError(
                "Invalid for statement: expected ',' after 'for end', got '{}'",
                it->value));
        }
        ++it; // Skip ','

        // Parse step expression
        // Must be either an expression or empty
        auto step = [&]() -> std::unique_ptr<ASTExpression> {
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
        if(it->type == TOKEN_PUNCT_PAREN_CLOSE)
        {
            ++it; // Skip ')'
        }

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

        return std::make_unique<ASTForStatement>(
            std::move(block), std::move(init), std::move(end), std::move(step));
    }

    std::unique_ptr<ASTVariableDefinitionExpression>
    Parser::parseVariableDefinition()
    {
        // Variable definition syntax:
        // let [ qualifiers ] name [ : type ] [ = init-expression ] ;
        // var [ qualifiers ] name [ : type ] [ = init-expression ] ;

        bool mut = false;
        if(it->type == TOKEN_KEYWORD_VAR)
        {
            mut = true;
        }
        ++it; // Skip 'let' / 'var'

        if(it->type == TOKEN_KEYWORD_MUT)
        {
            if(mut)
            {
                return parserError(
                    "Unexpected 'mut' after 'var' in variable definition");
            }
            mut = true;
            ++it; // Skip 'mut'
        }

        if(it->type != TOKEN_IDENTIFIER)
        {
            return parserError("Invalid variable definition: expected "
                               "identifier, got '{}' instead",
                               it->value);
        }
        std::string name = it->value;
        ++it; // Skip name

        std::string typen = "";
        if(it->type == TOKEN_PUNCT_COLON)
        {
            ++it; // Skip ':'

            if(it->type != TOKEN_IDENTIFIER)
            {
                return parserError("Invalid variable definition: expected "
                                   "identifier after ':', got '{}' instead",
                                   it->value);
            }

            typen = it->value;
            ++it; // Skip type
        }

        // Parse possible init expression
        auto init = [&]() -> std::unique_ptr<ASTExpression> {
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
            return nullptr;
        }

        auto name_ = std::make_unique<ASTIdentifierExpression>(name);

        // Type will be inferred by the code generator
        if(typen.empty())
        {
            if(!init || init->nodeType == ASTNode::EMPTY_EXPR)
            {
                return parserError("Invalid variable definition: init "
                                   "expression is required when type is not "
                                   "stated explicitly");
            }
            auto def = std::make_unique<ASTVariableDefinitionExpression>(
                std::move(name_), std::move(init));
            def->isMutable = mut;
            return def;
        }

        auto typename_ = std::make_unique<ASTIdentifierExpression>(typen);
        auto def = std::make_unique<ASTVariableDefinitionExpression>(
            std::move(typename_), std::move(name_), std::move(init));
        def->isMutable = mut;
        return def;
    }

    std::unique_ptr<ASTGlobalVariableDefinitionExpression>
    Parser::parseGlobalVariableDefinition()
    {
        auto var = parseVariableDefinition();
        if(!var)
        {
            return nullptr;
        }
        return std::make_unique<ASTGlobalVariableDefinitionExpression>(
            std::move(var));
    }

    std::unique_ptr<ASTModuleStatement> Parser::parseModuleStatement()
    {
        // Module statement syntax
        // "module" identifier ;

        ++it; // Skip 'module'

        if(it->type != TOKEN_IDENTIFIER)
        {
            return parserError("Invalid module statement: expected identifier "
                               "after 'module', got '{}' instead",
                               it->value);
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
                return parserError("Invalid module statement: expected "
                                   "identifier, got '{}' instead",
                                   it->value);
            }
            ++it;

            if(it->type == TOKEN_OPERATORB_MEMBER)
            {
                name += it->value;
                if(std::next(it)->type == TOKEN_EOF ||
                   std::next(it)->type == TOKEN_PUNCT_SEMICOLON)
                {
                    return parserError("Invalid module statement: expected "
                                       "identifier after '.', got '{}' instead",
                                       it->value);
                }
            }
            else if(it->type == TOKEN_PUNCT_SEMICOLON)
            {
                ++it; // Skip ';'
                break;
            }
            else
            {
                return parserError("Invalid module statement: expected '.' or "
                                   "';', got '{}' instead",
                                   it->value);
            }
            ++it; // Skip '.'
        }

        auto moduleName = std::make_unique<ASTIdentifierExpression>(name);
        return std::make_unique<ASTModuleStatement>(std::move(moduleName));
    }

    std::unique_ptr<ASTReturnStatement> Parser::parseReturnStatement()
    {
        ++it; // Skip return

        if(it->type == TOKEN_PUNCT_SEMICOLON)
        {
            ++it; // Skip ';'
            return std::make_unique<ASTReturnStatement>(
                std::make_unique<ASTEmptyExpression>());
        }

        auto expr = parseExpression();
        if(!expr)
        {
            return nullptr;
        }
        if(it->type != TOKEN_PUNCT_SEMICOLON)
        {
            return parserError(
                "Expected ';' after return statement, got '{}' instead",
                it->value);
        }
        ++it; // Skip ';'

        return std::make_unique<ASTReturnStatement>(std::move(expr));
    }

    std::unique_ptr<ASTExpression> Parser::parseIdentifierExpression()
    {
        std::string idName = it->value;
        ++it; // Skip identifier

        // If there is an identifier or 'mut' after the
        // identifier,
        // it's a variable definition
        if(it->type == TOKEN_IDENTIFIER || it->type == TOKEN_KEYWORD_MUT)
        {
            --it; // Roll back to the previous identifier, which is the typename
            return parseVariableDefinition();
        }

        auto id = std::make_unique<ASTIdentifierExpression>(idName);

        // Member access
        if(it->type == TOKEN_OPERATORB_MEMBER)
        {
            ++it; // Skip '.'
            auto rhs = parseExpression();
            return std::make_unique<ASTMemberAccessExpression>(std::move(id),
                                                               std::move(rhs));
        }

        // Subscript
        if(it->type == TOKEN_PUNCT_SQR_OPEN)
        {
            return parseSubscriptExpression(std::move(id));
        }

        // Function call
        if(it->type == TOKEN_PUNCT_PAREN_OPEN)
        {
            return parseFunctionCallExpression(std::move(id));
        }

        auto parseAssignment = [&]() -> std::unique_ptr<ASTExpression> {
            ++it; // Skip assignment operator
            return parseExpression();
        };

        auto varref = std::make_unique<ASTVariableRefExpression>(idName);
        std::unique_ptr<ASTAssignmentOperationExpression> assignment = nullptr;
        if(isAssignmentOperator())
        {
            auto op = it;
            auto rhs = parseAssignment();
            if(!rhs)
            {
                return nullptr;
            }
            assignment = std::make_unique<ASTAssignmentOperationExpression>(
                std::move(varref), std::move(rhs), op->type);
        }
        if(!assignment)
        {
            return std::move(varref);
        }
        return std::move(assignment);
    }

    std::unique_ptr<ast::ASTArbitraryOperationExpression>
    Parser::parseFunctionCallExpression(std::unique_ptr<ASTExpression> lhs)
    {
        ++it; // Skip '('
        std::vector<std::unique_ptr<ASTExpression>> operands;
        operands.push_back(std::move(lhs));
        if(it->type != TOKEN_PUNCT_PAREN_CLOSE)
        {
            // Parse argument list
            while(it->type != TOKEN_EOF)
            {
                if(auto arg = parseExpression())
                {
                    operands.push_back(std::move(arg));
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
                    return parserError("Invalid function call: Expected ')' or "
                                       "',' in argument list, got '{}' instead",
                                       it->value);
                }
                ++it; // Skip ','
            }
        }

        ++it; // Skip ')'
        return std::make_unique<ASTArbitraryOperationExpression>(
            std::move(operands), lexer::TOKEN_OPERATORC_CALL);
    }

    std::unique_ptr<ASTSubscriptExpression>
    Parser::parseSubscriptExpression(std::unique_ptr<ASTExpression> lhs)
    {
        ++it; // Skip '['
        auto subscr = parseExpression();
        if(!subscr)
        {
            return nullptr;
        }

        if(it->type != TOKEN_PUNCT_SQR_CLOSE)
        {
            return parserError("Invalid subscript expression: Expected ']' "
                               "after expression, got '{}' instead",
                               it->value);
        }
        ++it; // Skip ']'

        return std::make_unique<ASTSubscriptExpression>(std::move(lhs),
                                                        std::move(subscr));
    }

    std::unique_ptr<ASTCastExpression> Parser::parseCastExpression()
    {
        ++it; // Skip 'cast'

        if(it->type != TOKEN_OPERATORB_LESS)
        {
            return parserError(
                "Invalid cast: Expected '<' after 'cast', got '{}' instead",
                it->value);
        }
        ++it; // Skip '<'

        if(it->type != TOKEN_IDENTIFIER)
        {
            return parserError(
                "Invalid cast: Expected identifier after '<', got '{}' instead",
                it->value);
        }
        auto type = std::make_unique<ASTIdentifierExpression>(it->value);
        ++it; // Skip identifier

        if(it->type != TOKEN_OPERATORB_GREATER)
        {
            return parserError(
                "Invalid cast: Expected '>' after identifier, got '{}' instead",
                it->value);
        }
        ++it; // Skip '>'

        if(it->type != TOKEN_PUNCT_PAREN_OPEN)
        {
            return parserError(
                "Invalid cast: Expected '(' after '>', got '{}' instead",
                it->value);
        }
        ++it; // Skip '('

        auto castee = parseExpression();

        if(it->type != TOKEN_PUNCT_PAREN_CLOSE)
        {
            return parserError(
                "Invalid cast: Expected ')' after identifier, got '{}' instead",
                it->value);
        }
        ++it; // Skip ')'

        return std::make_unique<ASTCastExpression>(std::move(castee),
                                                   std::move(type));
    }

    std::unique_ptr<ASTExpression>
    Parser::parsePrimary(bool tolerateUnrecognized)
    {
        // Parse primary expression
        switch(it->type.get())
        {
        case TOKEN_IDENTIFIER:
            return parseIdentifierExpression();
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
        case TOKEN_KEYWORD_CAST:
            return parseCastExpression();
        default:
            if(tolerateUnrecognized)
            {
                return nullptr;
            }
            return parserError("Unknown token: '{}'", it->value);
        }
    }

    std::unique_ptr<ASTIntegerLiteralExpression>
    Parser::parseIntegerLiteralExpression()
    {
        const int base = [&]() {
            if(it->modifierInt.isSet(INTEGER_HEX))
            {
                return 16;
            }
            if(it->modifierInt.isSet(INTEGER_OCT))
            {
                return 8;
            }
            if(it->modifierInt.isSet(INTEGER_BIN))
            {
                return 2;
            }
            return 10;
        }();

        const TokenVector::const_iterator lit = it;
        ++it;

        try
        {
            int64_t val = std::stoll(lit->value, nullptr, base);
            std::string type;
            if(lit->modifierInt.isSet(INTEGER_INT))
            {
                type = "int";
                if(val > std::numeric_limits<int32_t>::max() ||
                   val < std::numeric_limits<int32_t>::min())
                {
                    throw std::out_of_range(
                        fmt::format("'{}' cannot fit into int", lit->value));
                }
            }
            else if(lit->modifierInt.isSet(INTEGER_INT8))
            {
                type = "int8";
                if(val > std::numeric_limits<int8_t>::max() ||
                   val < std::numeric_limits<int8_t>::min())
                {
                    throw std::out_of_range(
                        fmt::format("'{}' cannot fit into int8", lit->value));
                }
            }
            else if(lit->modifierInt.isSet(INTEGER_INT16))
            {
                type = "int16";
                if(val > std::numeric_limits<int16_t>::max() ||
                   val < std::numeric_limits<int16_t>::min())
                {
                    throw std::out_of_range(
                        fmt::format("'{}' cannot fit into int16", lit->value));
                }
            }
            else if(lit->modifierInt.isSet(INTEGER_INT32))
            {
                type = "int32";
                if(val > std::numeric_limits<int32_t>::max() ||
                   val < std::numeric_limits<int32_t>::min())
                {
                    throw std::out_of_range(
                        fmt::format("'{}' cannot fit into int32", lit->value));
                }
            }
            else if(lit->modifierInt.isSet(INTEGER_INT64))
            {
                type = "int64";
            }
            else if(lit->modifierInt.isSet(INTEGER_BYTE))
            {
                type = "byte";
                if(val > std::numeric_limits<uint8_t>::max() ||
                   val < std::numeric_limits<uint8_t>::min())
                {
                    throw std::out_of_range(
                        fmt::format("'{}' cannot fit into byte", lit->value));
                }
            }
            /*else if(lit->modifierInt.isSet(INTEGER_BIN) ||
                    lit->modifierInt.isSet(INTEGER_OCT) ||
                    lit->modifierInt.isSet(INTEGER_HEX))
            {
            }*/
            else
            {
                throw std::invalid_argument(fmt::format(
                    "Invalid integer modifier: {}", lit->modifierInt.get()));
            }
            return std::make_unique<ASTIntegerLiteralExpression>(
                val, std::make_unique<ASTIdentifierExpression>(type));
        }
        catch(std::invalid_argument& e)
        {
            return parserError("Invalid integer literal: literal is "
                               "ill-formed: '{}'. Description: '{}'",
                               lit->value, e.what());
        }
        catch(std::out_of_range& e)
        {
            return parserError("Invalid integer literal: value out of range: "
                               "'{}'. Description: '{}'",
                               lit->value, e.what());
        }
    }
    std::unique_ptr<ASTFloatLiteralExpression>
    Parser::parseFloatLiteralExpression()
    {
        const TokenVector::const_iterator lit = it;
        ++it;

        try
        {
            double val;
            std::string type;
            std::tie(val, type) = [&]() -> std::tuple<double, std::string> {
                if(lit->modifierFloat.isSet(FLOAT_F64))
                {
                    return std::make_tuple<double, std::string>(
                        std::stod(lit->value), "f64");
                }
                if(lit->modifierFloat.isSet(FLOAT_F32))
                {
                    return std::make_tuple<double, std::string>(
                        static_cast<double>(std::stof(lit->value)), "f32");
                }
                if(lit->modifierFloat.isSet(FLOAT_FLOAT))
                {
                    return std::make_tuple<double, std::string>(
                        std::stod(lit->value), "float");
                }
                throw std::invalid_argument(fmt::format(
                    "Invalid float modifier: {}", lit->modifierFloat.get()));
            }();
            return std::make_unique<ASTFloatLiteralExpression>(
                val, std::make_unique<ASTIdentifierExpression>(type));
        }
        catch(std::invalid_argument& e)
        {
            return parserError("Invalid float literal: literal is ill-formed: "
                               "'{}'. Description: '{}'",
                               lit->value, e.what());
        }
        catch(std::out_of_range& e)
        {
            return parserError("Invalid float literal: value out of range: "
                               "'{}'. Description: '{}'",
                               lit->value, e.what());
        }
    }

    std::unique_ptr<ASTStringLiteralExpression>
    Parser::parseStringLiteralExpression()
    {
        const TokenVector::const_iterator lit = it;
        ++it;

        return std::make_unique<ASTStringLiteralExpression>(lit->value);
    }

    std::unique_ptr<ASTCharLiteralExpression>
    Parser::parseCharLiteralExpression()
    {
        const TokenVector::const_iterator lit = it;
        ++it;

        std::vector<char32_t> result;
        utf8::utf8to32(lit->value.begin(), lit->value.end(),
                       std::back_inserter(result));

        if(result.size() > 1)
        {
            return parserError("Invalid char literal: Value out of range: "
                               "Length more than one Unicode character (4 "
                               "bytes): '{}'",
                               lit->value);
        }
        assert(result.size() == 1);

        return std::make_unique<ASTCharLiteralExpression>(result[0]);
        /*char32_t val = 0;
        bool set = false;
        for(int i = 3; i >= 0; --i)
        {
            std::string chbuf(lit->value.begin(), lit->value.begin() + i);
            int64_t tmp = 0;

        }
        std::string ch
        auto val = lit->value.at(0);
        return std::make_unique<ASTCharLiteralExpression>(
            static_cast<char32_t>(val));*/
    }
    std::unique_ptr<ASTBoolLiteralExpression>
    Parser::parseTrueLiteralExpression()
    {
        ++it;
        return std::make_unique<ASTBoolLiteralExpression>(true);
    }
    std::unique_ptr<ASTBoolLiteralExpression>
    Parser::parseFalseLiteralExpression()
    {
        ++it;
        return std::make_unique<ASTBoolLiteralExpression>(false);
    }
    std::unique_ptr<ASTNoneLiteralExpression>
    Parser::parseNoneLiteralExpression()
    {
        ++it;
        return std::make_unique<ASTNoneLiteralExpression>();
    }

    std::unique_ptr<ASTExpression> Parser::parseExpression()
    {
        std::stack<core::lexer::TokenType> operators;
        std::stack<std::unique_ptr<ASTExpression>> operands;
        int parenCount = 0;
        const auto beginExprIt = it;

        while(it != endTokens)
        {
            if(it->type == TOKEN_PUNCT_PAREN_OPEN)
            {
                operators.push(it->type);
                ++it; // Skip '('
                ++parenCount;
                continue;
            }
            else if(it->type == TOKEN_PUNCT_PAREN_CLOSE)
            {
                if(parenCount == 0)
                {
                    break;
                }

                ++it; // Skip ')'
                --parenCount;
                if(parenCount >= 0)
                {
                    bool ok = false;
                    while(!operators.empty())
                    {
                        if(operators.top() == TOKEN_PUNCT_PAREN_OPEN)
                        {
                            operators.pop();
                            ok = true;
                            break;
                        }
                        else
                        {
                            auto rhs = std::move(operands.top());
                            operands.pop();
                            auto lhs = std::move(operands.top());
                            operands.pop();

                            auto expr =
                                std::make_unique<ASTBinaryOperationExpression>(
                                    std::move(lhs), std::move(rhs),
                                    operators.top());
                            operands.push(std::move(expr));

                            operators.pop();
                        }
                    }
                    if(ok)
                    {
                        continue;
                    }
                }

                return parserError("Parenthesis mismatch in expression");
            }
            else if(isBinaryOperator())
            {
                if(it == beginExprIt || getBinOpPrecedence(it - 1) >= 0)
                {
                    if(it->type == TOKEN_OPERATORB_ADD)
                    {
                        operators.push(Token::create(TOKEN_OPERATORU_PLUS,
                                                     it->value, it->loc)
                                           .type);
                        ++it;
                        continue;
                    }
                    else if(it->type == TOKEN_OPERATORB_SUB)
                    {
                        operators.push(Token::create(TOKEN_OPERATORU_MINUS,
                                                     it->value, it->loc)
                                           .type);
                        ++it;
                        continue;
                    }
                }

                if(operators.empty() ||
                   operators.top() == TOKEN_PUNCT_PAREN_OPEN ||
                   (getBinOpPrecedence() >
                    getBinOpPrecedence(operators.top())) ||
                   (isBinOpRightAssociative() &&
                    getBinOpPrecedence() ==
                        getBinOpPrecedence(operators.top())))
                {
                    operators.push(it->type);
                    ++it; // Skip operator
                    continue;
                }
                else if(getBinOpPrecedence() <
                            getBinOpPrecedence(operators.top()) ||
                        (!isBinOpRightAssociative() &&
                         getBinOpPrecedence() ==
                             getBinOpPrecedence(operators.top())))
                {
                    while(getBinOpPrecedence() <
                              getBinOpPrecedence(operators.top()) ||
                          (!isBinOpRightAssociative() &&
                           getBinOpPrecedence() ==
                               getBinOpPrecedence(operators.top())))
                    {
                        auto rhs = std::move(operands.top());
                        operands.pop();
                        auto lhs = std::move(operands.top());
                        operands.pop();

                        auto expr =
                            std::make_unique<ASTBinaryOperationExpression>(
                                std::move(lhs), std::move(rhs),
                                operators.top());
                        operands.push(std::move(expr));

                        operators.pop();
                        if(operators.empty())
                        {
                            break;
                        }
                    }
                    operators.push(it->type);
                    ++it;
                }
                else
                {
                    return parserError("No sufficent operator rule found");
                }
            }
            else if(isPrefixUnaryOperator())
            {
                operators.push(it->type);
                ++it; // Skip operator
                continue;
            }
            else if(it->type == TOKEN_IDENTIFIER)
            {
                auto expr = parseIdentifierExpression();
                if(!expr)
                {
                    return nullptr;
                }
                operands.push(std::move(expr));
                continue;
            }
            else if(it->type == TOKEN_PUNCT_SEMICOLON ||
                    it->type == TOKEN_PUNCT_BRACE_OPEN ||
                    it->type == TOKEN_PUNCT_SQR_OPEN ||
                    it->type == TOKEN_PUNCT_COMMA)
            {
                break;
            }
            else if(it->type == TOKEN_EOF)
            {
                return parserError("Unexpected EOF in expression");
            }
            else
            {
                auto primary = parsePrimary(true);
                if(!primary)
                {
                    if(operators.empty())
                    {
                        break;
                    }
                    return parserError("Invalid token in expression: {} ({})",
                                       it->value, it->typeToString());
                }
                operands.push(std::move(primary));
                continue;
            }
        }

        while(!operators.empty())
        {
            if(isUnaryOperator(operators.top()))
            {
                if(operands.empty())
                {
                    return parserError("Unary operators require 1 operand");
                }
                auto rhs = std::move(operands.top());
                operands.pop();

                auto expr = std::make_unique<ASTUnaryOperationExpression>(
                    std::move(rhs), operators.top());
                operands.push(std::move(expr));

                operators.pop();
            }
            else
            {
                if(operands.size() < 2)
                {
                    return parserError("Binary operators require 2 operands");
                }
                auto rhs = std::move(operands.top());
                operands.pop();
                auto lhs = std::move(operands.top());
                operands.pop();

                auto expr = std::make_unique<ASTBinaryOperationExpression>(
                    std::move(lhs), std::move(rhs), operators.top());
                operands.push(std::move(expr));

                operators.pop();
            }
        }

        if(operands.empty())
        {
            parserWarning("Expression evaluated as empty");
            return std::make_unique<ASTEmptyExpression>();
        }
        auto top = std::move(operands.top());
        return top;
    }

    void Parser::handleImport()
    {
        if(auto import = parseImportStatement())
        {
            util::logger->trace("Parsed import statement: importee: '{}', "
                                "isPath: '{}', type: '{}'",
                                import->importee->value, import->isPath,
                                static_cast<int>(import->importType));
            getAST().pushStatement(std::move(import));
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
            getAST().pushStatement(std::move(module));
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
            util::logger->trace("Parsed function definition: name: '{}', "
                                "return: '{}', params.size: '{}'",
                                def->proto->name->value,
                                def->proto->returnType->value,
                                def->proto->params.size());
            getAST().pushStatement(std::move(def));
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
            getAST().pushStatement(std::move(stmt));
        }
        else
        {
            ++it;
        }
    }

    void Parser::handleGlobalVariable()
    {
        if(auto expr = parseGlobalVariableDefinition())
        {
            util::logger->trace("Parsed global variable definition: name: "
                                "'{}', type: '{}', isMutable: '{}'",
                                expr->var->name->value,
                                expr->var->typeInferred ? expr->var->type->value
                                                        : "(will be inferred)",
                                expr->var->isMutable);
            getAST().pushStatement(wrapExpression(std::move(expr)));
        }
        else
        {
            ++it;
        }
    }

    void Parser::handleExport()
    {
        ++it; // Skip 'export'

        if(it->type == TOKEN_KEYWORD_LET || it->type == TOKEN_KEYWORD_VAR)
        {
            auto expr = parseGlobalVariableDefinition();
            if(!expr)
            {
                return;
            }

            expr->isExport = true;

            util::logger->trace("Parsed global variable definition: name: "
                                "'{}', type: '{}', isMutable: '{}'",
                                expr->var->name->value,
                                expr->var->typeInferred ? expr->var->type->value
                                                        : "(will be inferred)",
                                expr->var->isMutable);
            getAST().pushStatement(wrapExpression(std::move(expr)));
        }
        else if(it->type == TOKEN_KEYWORD_DEFINE)
        {
            auto def = parseFunctionDefinitionStatement();
            if(!def)
            {
                return;
            }

            def->isExport = true;

            util::logger->trace("Parsed function definition: name: '{}', "
                                "return: '{}', params.size: '{}'",
                                def->proto->name->value,
                                def->proto->returnType->value,
                                def->proto->params.size());
            getAST().pushStatement(std::move(def));
        }
        else
        {
            util::logger->error("Unexpected token after 'export': '{}'",
                                it->value);
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
        case TOKEN_KEYWORD_WHILE:
            return parseWhileStatement();

        case TOKEN_KEYWORD_RETURN:
            return parseReturnStatement();

        case TOKEN_KEYWORD_VAR:
        case TOKEN_KEYWORD_LET:
        case TOKEN_KEYWORD_MUT:
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

    std::unique_ptr<ast::ASTFunctionParameter> Parser::parseFunctionParameter()
    {
        if(it->type != TOKEN_IDENTIFIER)
        {
            return parserError("Invalid function parameter: expected "
                               "identifier, got '{}' instead",
                               it->value);
        }
        std::string name = it->value;
        ++it; // Skip name

        if(it->type != TOKEN_PUNCT_COLON)
        {
            return parserError("Invalid function parameter: expected "
                               "':' after identifier, got '{}' instead",
                               it->value);
        }
        ++it; // Skip ':'

        std::string typen = it->value;
        ++it; // Skip type

        // Parse possible init expression
        auto init = [&]() -> std::unique_ptr<ASTExpression> {
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
            return nullptr;
        }

        auto typeExpr = std::make_unique<ASTIdentifierExpression>(typen);
        auto nameExpr = std::make_unique<ASTIdentifierExpression>(name);
        auto var = std::make_unique<ASTVariableDefinitionExpression>(
            std::move(typeExpr), std::move(nameExpr), std::move(init));

        return std::make_unique<ASTFunctionParameter>(std::move(var));
    }

    std::unique_ptr<ASTFunctionPrototypeStatement>
    Parser::parseFunctionPrototype()
    {
        if(it->type != TOKEN_IDENTIFIER)
        {
            return parserError("Invalid function prototype: expected "
                               "identifier, got '{}' instead",
                               it->value);
        }
        auto funcName = std::make_unique<ASTIdentifierExpression>(it->value);
        ++it; // Skip identifier

        if(it->type != TOKEN_PUNCT_PAREN_OPEN)
        {
            return parserError(
                "Invalid function prototype: expected '(', got '{}' instead",
                it->value);
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

            auto param = parseFunctionParameter();
            if(!param)
            {
                return nullptr;
            }
            params.push_back(std::move(param));

            if(it->type == TOKEN_EOF)
            {
                return parserError("Invalid function prototype: expected ')' "
                                   "or ',' in parameter list, got '{}' instead",
                                   it->value);
            }
            if(it->type == TOKEN_PUNCT_COMMA)
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
                return parserError("Invalid function prototype: expected ')' "
                                   "or ',' in parameter, got '{}' instead",
                                   it->value);
            }
        }

        if(it->type != TOKEN_PUNCT_COLON)
        {
            return parserError("Invalid function prototype: expected ':' "
                               "before return type, got '{}' instead",
                               it->value);
        }
        ++it; // Skip ':'

        if(it->type != TOKEN_IDENTIFIER)
        {
            return parserError("Invalid function prototype: expected "
                               "identifier in return type, got '{}' instead",
                               it->value);
        }
        auto returnType = std::make_unique<ASTIdentifierExpression>(it->value);
        ++it; // Skip identifier

        return std::make_unique<ASTFunctionPrototypeStatement>(
            std::move(funcName), std::move(returnType), std::move(params));
    }

    std::unique_ptr<ASTFunctionDefinitionStatement>
    Parser::parseFunctionDefinitionStatement()
    {
        ++it; // Skip 'def'
        auto proto = parseFunctionPrototype();
        if(!proto)
        {
            return nullptr;
        }

        // Just a declaration
        if(it->type == TOKEN_PUNCT_SEMICOLON)
        {
            ++it; // Skip ';'
            auto body = std::make_unique<ASTEmptyStatement>();
            return std::make_unique<ASTFunctionDefinitionStatement>(
                std::move(proto), std::move(body));
        }

        if(it->type != TOKEN_PUNCT_BRACE_OPEN)
        {
            return parserError("Invalid function definition: expected '{' to "
                               "start function body, got '{}' instead",
                               it->value);
        }
        if(auto body = parseBlockStatement())
        {
            return std::make_unique<ASTFunctionDefinitionStatement>(
                std::move(proto), std::move(body));
        }
        return nullptr;
    }

    std::unique_ptr<ASTWrappedExpressionStatement>
    Parser::wrapExpression(std::unique_ptr<ASTExpression> expr)
    {
        if(!expr)
        {
            return nullptr;
        }
        if(it->type != TOKEN_PUNCT_SEMICOLON)
        {
            return parserError("Expected semicolon after expression statement, "
                               "got '{}' instead",
                               it->value);
        }
        ++it;
        return std::make_unique<ASTWrappedExpressionStatement>(std::move(expr));
    }

    bool Parser::isPrefixUnaryOperator() const
    {
        return isPrefixUnaryOperator(it->type);
    }

    bool Parser::isPrefixUnaryOperator(const core::lexer::TokenType& op) const
    {
        return (op == TOKEN_OPERATORU_PLUS || op == TOKEN_OPERATORU_MINUS ||
                op == TOKEN_OPERATORU_NOT || op == TOKEN_OPERATORU_SIZEOF ||
                op == TOKEN_OPERATORU_TYPEOF || op == TOKEN_OPERATORU_ADDROF);
    }

    bool Parser::isPostfixUnaryOperator() const
    {
        return isPostfixUnaryOperator(it->type);
    }

    bool Parser::isPostfixUnaryOperator(const core::lexer::TokenType& op) const
    {
        return (op == TOKEN_OPERATORU_INC || op == TOKEN_OPERATORU_DEC);
    }

    bool Parser::isUnaryOperator() const
    {
        return isUnaryOperator(it->type);
    }

    bool Parser::isUnaryOperator(const core::lexer::TokenType& op) const
    {
        return isPrefixUnaryOperator(op) || isPostfixUnaryOperator(op);
    }

    bool Parser::isBinaryOperator() const
    {
        return getBinOpPrecedence() >= 10;
    }

    bool Parser::isOperator() const
    {
        return isBinaryOperator() || isUnaryOperator();
    }

    int Parser::getBinOpPrecedence() const
    {
        return getBinOpPrecedence(it);
    }

    int Parser::getBinOpPrecedence(
        core::lexer::TokenVector::const_iterator op) const
    {
        return getBinOpPrecedence(op->type);
    }

    int Parser::getBinOpPrecedence(const core::lexer::TokenType& t) const
    {
        using namespace core::lexer;

        switch(t.get())
        {
        case TOKEN_OPERATORB_OR:
            return 10;
        case TOKEN_OPERATORB_AND:
            return 20;

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
        case TOKEN_OPERATORB_REM:
            return 100;
        case TOKEN_OPERATORB_POW:
            return 110;

        case TOKEN_OPERATORB_MEMBER:
            return 120;

        case TOKEN_OPERATORB_INSTOF:
            return 130;

        default:
            return -1;
        }
    }

    bool Parser::isAssignmentOperator() const
    {
        return isAssignmentOperator(it);
    }

    bool Parser::isAssignmentOperator(
        core::lexer::TokenVector::const_iterator op) const
    {
        using namespace core::lexer;

        switch(op->type.get())
        {
        case TOKEN_OPERATORA_SIMPLE:
        case TOKEN_OPERATORA_ADD:
        case TOKEN_OPERATORA_SUB:
        case TOKEN_OPERATORA_MUL:
        case TOKEN_OPERATORA_DIV:
        case TOKEN_OPERATORA_MOD:
        case TOKEN_OPERATORA_POW:
            return true;
        default:
            return false;
        }
    }

    bool Parser::isBinOpRightAssociative() const
    {
        return isBinOpRightAssociative(it);
    }

    bool Parser::isBinOpRightAssociative(
        core::lexer::TokenVector::const_iterator op) const
    {
        switch(op->type.get())
        {
        case TOKEN_OPERATORB_POW:
            return true;
        default:
            return false;
        }
    }
} // namespace parser
} // namespace core
