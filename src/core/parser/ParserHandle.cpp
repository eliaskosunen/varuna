// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#include "ast/ControlStmt.h"
#include "ast/FunctionStmt.h"
#include "core/parser/Parser.h"

namespace core
{
namespace parser
{
    using namespace lexer;
    using namespace ast;

    void Parser::handleImport()
    {
        if(auto import = parseImportStatement())
        {
            util::logger->trace("Parsed import statement: importee: '{}', "
                                "isPath: '{}', type: '{}'",
                                import->importee->value, import->isPath,
                                import->importType.get());
            getAST().push(std::move(import));
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
            getAST().push(std::move(module));
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
            getAST().push(std::move(def));
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
            getAST().push(std::move(stmt));
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
            getAST().push(createExprStmt(std::move(expr)));
        }
        else
        {
            ++it;
        }
    }

    void Parser::handleExport()
    {
        ++it; // Skip 'export'

        bool mangle = true;
        if(it->type == TOKEN_KEYWORD_NO_MANGLE)
        {
            ++it; // Skip 'nomangle'
            mangle = false;
        }

        if(it->type == TOKEN_KEYWORD_LET)
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
            getAST().push(createExprStmt(std::move(expr)));
        }
        else if(it->type == TOKEN_KEYWORD_DEFINE)
        {
            auto def = parseFunctionDefinitionStatement();
            if(!def)
            {
                return;
            }

            def->proto->isExport = true;
            def->proto->mangle = mangle;

            util::logger->trace("Parsed function definition: name: '{}', "
                                "return: '{}', params.size: '{}'",
                                def->proto->name->value,
                                def->proto->returnType->value,
                                def->proto->params.size());
            getAST().push(std::move(def));
        }
        else
        {
            util::logger->error("Unexpected token after 'export': '{}'",
                                it->value);
        }
    }

    void Parser::handleUse()
    {
        if(auto stmt = parseAliasStatement())
        {
            util::logger->trace(
                "Parsed alias statement: alias: '{}', aliasee: '{}'",
                stmt->alias->value, stmt->aliasee->value);
            getAST().push(std::move(stmt));
        }
        else
        {
            ++it;
        }
    }
} // namespace parser
} // namespace core
