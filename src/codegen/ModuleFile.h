// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#pragma once

#include "ast/AST.h"
#include "codegen/CodegenInfo.h"
#include "llvm/IR/IRBuilder.h"

#if 0
namespace codegen
{
    class Symbol;

    class ModuleFile
    {
    public:
        struct ModuleFileData
        {
            std::unique_ptr<ast::AST> ast;

            template <class Archive>
            void serialize(Archive& archive)
            {
                archive(ast);
            }
        };

        ModuleFile(const CodegenInfo& i, llvm::LLVMContext& c, llvm::Module* m,
                   llvm::IRBuilder<>& b);

        std::vector<std::unique_ptr<Symbol>> read();
        void write(std::vector<Symbol*>&& exportSymbols);

    private:
        const CodegenInfo& info;
        std::string modulefile;
        llvm::LLVMContext& context;
        llvm::Module* module;
        llvm::IRBuilder<>& builder;
    };
} // namespace codegen
#endif
