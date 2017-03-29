// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

// clang-format off
#include "core/parser/Parser.h"
// clang-format on
#include "ast/ControlStmt.h"
#include "ast/FunctionStmt.h"
#include "ast/LiteralExpr.h"

namespace core
{
namespace parser
{
    using namespace lexer;
    using namespace ast;

    std::unique_ptr<Stmt> Parser::parseStatement()
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

        case TOKEN_KEYWORD_LET:
        {
            auto expr = parseVariableDefinition();
            if(!expr)
            {
                return nullptr;
            }
            auto stmt = createExprStmt(std::move(expr));
            if(!stmt)
            {
                return nullptr;
            }
            return std::move(stmt);
        }
        case TOKEN_IDENTIFIER:
        {
            auto expr = parseExpression();
            if(!expr)
            {
                return nullptr;
            }
            auto stmt = createExprStmt(std::move(expr));
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

    std::unique_ptr<BlockStmt> Parser::parseBlockStatement()
    {
        ++it; // Skip '{'
        auto block = createNode<BlockStmt>(it - 1);
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

    std::unique_ptr<ImportStmt> Parser::parseImportStatement()
    {
        // Import statement syntax:
        // "import" [package/module] identifier/literal-string ;

        const auto iter = it;
        ++it; // Skip "import"

        // Parse import type
        // Options:
        //  * module
        //  * package
        // Not required
        ImportStmt::ImportType importType = ImportStmt::UNSPECIFIED;
        if(it->type == TOKEN_KEYWORD_MODULE)
        {
            importType = ImportStmt::MODULE;
            ++it;
        }
        else if(it->type == TOKEN_KEYWORD_PACKAGE)
        {
            importType = ImportStmt::PACKAGE;
            ++it;
        }

        // Parse module/file to import
        // Either an identifier or a string literal
        std::string toImport;
        bool isPath = false;
        const auto toImportIter = it;

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

        auto toImportObj =
            createNode<IdentifierExpr>(toImportIter, std::move(toImport));
        auto stmt = createNode<ImportStmt>(iter, importType,
                                           std::move(toImportObj), isPath);
        ++it;
        return stmt;
    }

    std::unique_ptr<ModuleStmt> Parser::parseModuleStatement()
    {
        // Module statement syntax
        // "module" identifier ;

        const auto iter = it;
        ++it; // Skip 'module'

        if(it->type != TOKEN_IDENTIFIER)
        {
            return parserError("Invalid module statement: expected identifier "
                               "after 'module', got '{}' instead",
                               it->value);
        }

        // Construct module name
        std::string name;
        const auto nameIter = it;
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

        auto moduleName = createNode<IdentifierExpr>(nameIter, std::move(name));
        return createNode<ModuleStmt>(iter, std::move(moduleName));
    }

    std::unique_ptr<IfStmt> Parser::parseIfStatement()
    {
        // If statement syntax:
        // "if" ( [expression] ) statement [ else statement ]

        const auto iter = it;
        ++it; // Skip 'if'

        // Parse condition
        if(it->type == TOKEN_PUNCT_PAREN_OPEN)
        {
            ++it; // Skip '('
        }
        auto cond = [&]() -> std::unique_ptr<Expr> {
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
        auto elsestmt = [&]() -> std::unique_ptr<Stmt> {
            if(it->type != TOKEN_KEYWORD_ELSE)
            {
                return createNode<EmptyStmt>(iter);
            }

            ++it; // Skip else
            return parseStatement();
        }();
        if(!elsestmt)
        {
            return nullptr;
        }

        return createNode<IfStmt>(iter, std::move(cond), std::move(then),
                                  std::move(elsestmt));
    }

    std::unique_ptr<WhileStmt> Parser::parseWhileStatement()
    {
        // While statement syntax:
        // "while" ( [expression] ) statement

        const auto iter = it;
        ++it; // Skip 'while'

        // Parse condition
        if(it->type == TOKEN_PUNCT_PAREN_OPEN)
        {
            ++it; // Skip '('
        }
        auto cond = [&]() -> std::unique_ptr<Expr> {
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

        return createNode<WhileStmt>(iter, std::move(cond), std::move(body));
    }

    Parser::ForCondition Parser::parseForCondition()
    {
        // Parse the condition of a for statement
        // [ ( ] [init] , [end] , [step] [ ) ]

        auto errorToTuple = [](std::nullptr_t e) {
            return ForCondition{e, e, e};
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
        auto init = [&]() -> std::unique_ptr<Expr> {
            if(it->type == TOKEN_PUNCT_COMMA)
            {
                return createNode<EmptyExpr>(it);
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
        auto end = [&]() -> std::unique_ptr<Expr> {
            if(it->type == TOKEN_PUNCT_COMMA)
            {
                // "true" by default
                return createNode<BoolLiteralExpr>(it, true);
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
        auto step = [&]() -> std::unique_ptr<Expr> {
            if(it->type == TOKEN_PUNCT_PAREN_CLOSE)
            {
                return createNode<EmptyExpr>(it);
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

        return {std::move(init), std::move(end), std::move(step)};
    }

    std::unique_ptr<ForStmt> Parser::parseForStatement()
    {
        // For-statement syntax
        // "for" () statement
        // "for" ( [expression] ; [expression] ; [expression] ) statement

        const auto iter = it;
        ++it; // Skip 'for'

        // TODO: Optimize
        // Not utilizing constructors
        // Not sure if possible in C++14
        ForCondition cond = parseForCondition();
        if(!cond.init || !cond.cond || !cond.step)
        {
            return nullptr;
        }

        // Parse statement
        std::unique_ptr<Stmt> block = parseStatement();
        if(!block)
        {
            return nullptr;
        }

        return createNode<ForStmt>(iter, std::move(block), std::move(cond.init),
                                   std::move(cond.cond), std::move(cond.step));
    }

    std::unique_ptr<AliasStmt> Parser::parseAliasStatement()
    {
        // 'use' alias '=' aliasee ';'

        const auto iter = it;
        ++it; // Skip 'use'

        if(it->type != TOKEN_IDENTIFIER)
        {
            return parserError(
                "Expected identifier after 'use', got '{}' instead", it->value);
        }
        auto aliasName = it->value;
        auto alias = createNode<IdentifierExpr>(it, std::move(aliasName));
        ++it; // Skip alias

        if(it->type != TOKEN_OPERATORA_SIMPLE)
        {
            return parserError(
                "Expected '=' after identifier, got '{}' instead", it->value);
        }
        ++it; // Skip '='

        if(it->type != TOKEN_IDENTIFIER)
        {
            return parserError(
                "Expected identifier after '=', got '{}' instead", it->value);
        }
        auto aliaseeName = it->value;
        auto aliasee = createNode<IdentifierExpr>(it, std::move(aliaseeName));
        ++it; // Skip aliasee

        if(it->type != TOKEN_PUNCT_SEMICOLON)
        {
            return parserError(
                "Expected ';' after use statement, got '{}' instead",
                it->value);
        }
        ++it; // Skip ';'

        return createNode<AliasStmt>(iter, std::move(alias),
                                     std::move(aliasee));
    }

    std::unique_ptr<ast::FunctionParameter>
    Parser::parseFunctionParameter(uint32_t num)
    {
        const auto iter = it;

        if(it->type != TOKEN_IDENTIFIER)
        {
            return parserError("Invalid function parameter: expected "
                               "identifier, got '{}' instead",
                               it->value);
        }
        std::string name = it->value;
        const auto nameIter = it;
        ++it; // Skip name

        if(it->type != TOKEN_PUNCT_COLON)
        {
            return parserError("Invalid function parameter: expected "
                               "':' after identifier, got '{}' instead",
                               it->value);
        }
        ++it; // Skip ':'

        std::string typen = it->value;
        const auto typeIter = it;
        ++it; // Skip type

        // Parse possible init expression
        auto init = [&]() -> std::unique_ptr<Expr> {
            if(it->type == TOKEN_OPERATORA_SIMPLE)
            {
                ++it; // Skip '='
                return parseExpression();
            }

            // No init expression
            return createNode<EmptyExpr>(it);
        }();
        if(!init)
        {
            return nullptr;
        }

        auto typeExpr = createNode<IdentifierExpr>(typeIter, std::move(typen));
        auto nameExpr = createNode<IdentifierExpr>(nameIter, std::move(name));
        auto var = createNode<VariableDefinitionExpr>(
            iter, std::move(typeExpr), std::move(nameExpr), std::move(init));

        return createNode<FunctionParameter>(iter, std::move(var), num);
    }

    std::unique_ptr<FunctionPrototypeStmt> Parser::parseFunctionPrototype()
    {
        const auto iter = it;

        if(it->type != TOKEN_IDENTIFIER)
        {
            return parserError("Invalid function prototype: expected "
                               "identifier, got '{}' instead",
                               it->value);
        }
        auto funcidvalue = it->value;
        auto funcName = createNode<IdentifierExpr>(it, std::move(funcidvalue));
        ++it; // Skip identifier

        if(it->type != TOKEN_PUNCT_PAREN_OPEN)
        {
            return parserError(
                "Invalid function prototype: expected '(', got '{}' instead",
                it->value);
        }
        ++it; // Skip '('

        std::vector<std::unique_ptr<FunctionParameter>> params;
        uint32_t paramNum = 0;
        while(true)
        {
            ++paramNum;

            if(it->type == TOKEN_PUNCT_PAREN_CLOSE)
            {
                ++it; // Skip ')'
                break;
            }

            auto param = parseFunctionParameter(paramNum);
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

        if(it->type == TOKEN_PUNCT_ARROW)
        {
            ++it; // Skip '->'

            if(it->type != TOKEN_IDENTIFIER)
            {
                return parserError(
                    "Invalid function prototype: expected "
                    "identifier in return type, got '{}' instead",
                    it->value);
            }
            auto idvalue = it->value;
            auto returnType =
                createNode<IdentifierExpr>(it, std::move(idvalue));
            ++it; // Skip identifier

            auto fnName = funcName->value;
            auto proto = createNode<FunctionPrototypeStmt>(
                iter, std::move(funcName), std::move(returnType),
                std::move(params));
            if(fnName == "main")
            {
                proto->isMain = true;
            }
            return proto;
        }
        auto fnName = funcName->value;
        auto returnType = createNode<IdentifierExpr>(it->loc - 1, "void");
        auto proto = createNode<FunctionPrototypeStmt>(
            iter, std::move(funcName), std::move(returnType),
            std::move(params));
        if(fnName == "main")
        {
            proto->isMain = true;
        }
        return proto;
    }

    std::unique_ptr<FunctionDefinitionStmt>
    Parser::parseFunctionDefinitionStatement()
    {
        const auto iter = it;

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
            auto body = createNode<EmptyStmt>(it - 1);
            return createNode<FunctionDefinitionStmt>(iter, std::move(proto),
                                                      std::move(body));
        }

        if(it->type != TOKEN_PUNCT_BRACE_OPEN)
        {
            parserError(it,
                        "Invalid function definition: expected '{{' or ';' to "
                        "start function body, got '{}' instead",
                        it->value);
            /*parserInfo(((it - 1)->loc) + 1,
                       "Add ';' (to make a declaration) or '{{' (to make a "
                       "definition) here");*/
            return nullptr;
        }
        if(auto body = parseBlockStatement())
        {
            return createNode<FunctionDefinitionStmt>(iter, std::move(proto),
                                                      std::move(body));
        }
        return nullptr;
    }

    std::unique_ptr<ReturnStmt> Parser::parseReturnStatement()
    {
        const auto iter = it;
        ++it; // Skip return

        if(it->type == TOKEN_PUNCT_SEMICOLON)
        {
            ++it; // Skip ';'
            return createNode<ReturnStmt>(iter, createNode<EmptyExpr>(iter));
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

        return createNode<ReturnStmt>(iter, std::move(expr));
    }

    std::unique_ptr<ExprStmt> Parser::createExprStmt(std::unique_ptr<Expr> expr)
    {
        const auto iter = it;
        if(!expr)
        {
            return nullptr;
        }
        if(it->type != TOKEN_PUNCT_SEMICOLON)
        {
            parserError(it, "Expected semicolon after expression statement, "
                            "got '{}' instead",
                        it->value);
            parserInfo(it, "Try adding a semicolon ';' here");
            return nullptr;
        }
        ++it;
        return createNode<ExprStmt>(iter, std::move(expr));
    }
} // namespace parser
} // namespace core
