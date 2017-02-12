// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#pragma once

#include "ast/ASTExpression.h"
#include "ast/ASTFunctionStatement.h"
#include "codegen/SymbolTable.h"
#include "util/File.h"
#include <cereal.h>

namespace codegen
{
class ModuleFile
{
public:
    struct ModuleFileSymbol
    {
        std::string typeName;
        std::string name;
        bool isMutable;
        util::SourceLocation loc;

        ModuleFileSymbol() = default;
        ModuleFileSymbol(const ModuleFileSymbol&) = delete;
        ModuleFileSymbol& operator=(const ModuleFileSymbol&) = delete;
        ModuleFileSymbol(ModuleFileSymbol&&) = default;
        ModuleFileSymbol& operator=(ModuleFileSymbol&&) = default;
        virtual ~ModuleFileSymbol() = default;

        template <class Archive>
        void serialize(Archive& archive)
        {
            archive(CEREAL_NVP(typeName), CEREAL_NVP(name),
                    CEREAL_NVP(isMutable), CEREAL_NVP(loc));
        }

        template <typename T, typename... Args>
        std::unique_ptr<T> createNode(util::SourceLocation l, ast::AST* ast,
                                      Args&&... args)
        {
            assert(ast);
            auto node = std::make_unique<T>(std::forward<Args>(args)...);
            node->loc = l;
            node->ast = ast;
            return node;
        }

        virtual std::unique_ptr<ast::ASTStatement> toASTNode(ast::AST* ast);
        virtual void fromSymbol(Symbol* s);
    };

    struct ModuleFileFunctionSymbol : ModuleFileSymbol
    {
        std::string retTypeName;
        std::vector<std::string> paramTypeNames;

        template <class Archive>
        void serialize(Archive& archive)
        {
            archive(cereal::base_class<ModuleFileSymbol>(this),
                    cereal::make_nvp("returnTypeName", retTypeName),
                    CEREAL_NVP(paramTypeNames));
        }

        std::unique_ptr<ast::ASTStatement> toASTNode(ast::AST* ast) override;
        void fromSymbol(Symbol* s) override;
    };

    struct ModuleFileSymbolTable
    {
        std::vector<std::unique_ptr<ModuleFileSymbol>> symbols;

        template <class Archive>
        void serialize(Archive& archive)
        {
            archive(CEREAL_NVP(symbols));
        }

        std::unique_ptr<ast::AST> toAST();
        void fromSymbolTable(std::unique_ptr<SymbolTable> s);

        static ModuleFileSymbolTable
        createFromSymbolTable(std::unique_ptr<SymbolTable> s)
        {
            ModuleFileSymbolTable t;
            t.fromSymbolTable(std::move(s));
            return t;
        }
    };

    ModuleFile(std::string file);

    ModuleFileSymbolTable read();
    void write(ModuleFileSymbolTable&& symbols);

private:
    std::string modulefile;
};
} // namespace codegen

#include <cereal_archives.h>

CEREAL_REGISTER_TYPE(codegen::ModuleFile::ModuleFileFunctionSymbol)
