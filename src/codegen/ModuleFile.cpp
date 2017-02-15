// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#include "codegen/ModuleFile.h"
#include "ast/AST.h"
#include "ast/ASTParentSolverVisitor.h"
#include "codegen/CodegenVisitor.h"
#include "codegen/Symbol.h"
#include "codegen/TypeTable.h"
#include "util/Compatibility.h"
#include <fstream>

#define ALWAYS_USE_BINARY_MODULE 1

#if VARUNA_DEBUG && !ALWAYS_USE_BINARY_MODULE
#define INPUT_ARCHIVE cereal::XMLInputArchive
#define OUTPUT_ARCHIVE cereal::XMLOutputArchive
#else
#define INPUT_ARCHIVE cereal::BinaryInputArchive
#define OUTPUT_ARCHIVE cereal::BinaryOutputArchive
#endif

namespace codegen
{
ModuleFile::ModuleFile(std::string file) : modulefile(std::move(file))
{
}

ModuleFile::ModuleFileSymbolTable ModuleFile::read()
{
    std::ifstream is;
    // is.exceptions(std::ios::badbit | std::ios::failbit);
    is.open(modulefile, std::ios::binary | std::ios::in);
    if(!is.is_open() || !is.good())
    {
        throw std::runtime_error(fmt::format(
            "Failed to open module file '{}' for reading", modulefile));
    }

    INPUT_ARCHIVE archive(is);

    ModuleFileSymbolTable data;
    archive(data);

    return data;
}

void ModuleFile::write(ModuleFile::ModuleFileSymbolTable&& symbols)
{
    std::ofstream os;
    // os.exceptions(std::ios::badbit | std::ios::failbit);
    os.open(modulefile, std::ios::binary | std::ios::out);
    if(!os.is_open() || !os.good())
    {
        throw std::runtime_error(fmt::format(
            "Failed to open module file '{}' for writing", modulefile));
    }

    OUTPUT_ARCHIVE archive(os);
    archive(symbols);
}

std::unique_ptr<ast::ASTStatement>
ModuleFile::ModuleFileSymbol::toASTNode(ast::AST* ast)
{
    auto vardef = createNode<ast::ASTVariableDefinitionExpression>(
        loc, ast,
        createNode<ast::ASTIdentifierExpression>(loc, ast, std::move(typeName)),
        createNode<ast::ASTIdentifierExpression>(loc, ast, std::move(name)),
        createNode<ast::ASTEmptyExpression>(loc, ast));
    vardef->isMutable = isMutable;
    auto global = createNode<ast::ASTGlobalVariableDefinitionExpression>(
        loc, ast, std::move(vardef));
    return createNode<ast::ASTWrappedExpressionStatement>(loc, ast,
                                                          std::move(global));
}

void ModuleFile::ModuleFileSymbol::fromSymbol(Symbol* s)
{
    typeName = s->getType()->getDecoratedName();
    name = s->name;
    isMutable = s->isMutable;
    loc = s->loc;
}

std::unique_ptr<ast::ASTStatement>
ModuleFile::ModuleFileFunctionSymbol::toASTNode(ast::AST* ast)
{
    std::vector<std::unique_ptr<ast::ASTFunctionParameter>> paramTypes;
    for(size_t i = 0; i < paramTypeNames.size(); ++i)
    {
        auto& p = paramTypeNames[i];
        auto vardef = createNode<ast::ASTVariableDefinitionExpression>(
            loc, ast,
            createNode<ast::ASTIdentifierExpression>(loc, ast, std::move(p)),
            createNode<ast::ASTIdentifierExpression>(loc, ast, ""),
            createNode<ast::ASTEmptyExpression>(loc, ast));
        paramTypes.push_back(createNode<ast::ASTFunctionParameter>(
            loc, ast, std::move(vardef), i + 1));
    }
    auto proto = createNode<ast::ASTFunctionPrototypeStatement>(
        loc, ast,
        createNode<ast::ASTIdentifierExpression>(loc, ast, std::move(name)),
        createNode<ast::ASTIdentifierExpression>(loc, ast,
                                                 std::move(retTypeName)),
        std::move(paramTypes));
    return createNode<ast::ASTFunctionDefinitionStatement>(
        loc, ast, std::move(proto),
        createNode<ast::ASTEmptyStatement>(loc, ast));
}

void ModuleFile::ModuleFileFunctionSymbol::fromSymbol(Symbol* s)
{
    typeName = s->getType()->getDecoratedName();
    name = s->name;
    isMutable = s->isMutable;
    loc = s->loc;
    auto type = static_cast<FunctionType*>(s->getType());
    retTypeName = type->returnType->getDecoratedName();
    for(auto& pType : type->params)
    {
        paramTypeNames.push_back(pType->getDecoratedName());
    }
}

std::unique_ptr<ast::AST> ModuleFile::ModuleFileSymbolTable::toAST()
{
    if(symbols.empty())
    {
        return std::make_unique<ast::AST>(nullptr);
    }
    auto ast = std::make_unique<ast::AST>(symbols[0]->loc.file);
    for(auto& s : symbols)
    {
        ast->push(s->toASTNode(ast.get()));
    }
    {
        ast::ASTParentSolverVisitor p;
        p.run(ast->globalNode.get());
    }
    return ast;
}

void ModuleFile::ModuleFileSymbolTable::fromSymbolTable(
    std::unique_ptr<SymbolTable> s)
{
    auto list = s->consumeList();
    if(list.size() == 0)
    {
        return;
    }
    for(auto& symbol : list[0])
    {
        if(symbol.second->isFunction())
        {
            auto fs = std::make_unique<ModuleFileFunctionSymbol>();
            ;
            fs->fromSymbol(symbol.second.get());
            symbols.push_back(std::move(fs));
        }
        else
        {
            auto sy = std::make_unique<ModuleFileSymbol>();
            sy->fromSymbol(symbol.second.get());
            symbols.push_back(std::move(sy));
        }
    }
}
} // namespace codegen
