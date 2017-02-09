// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#include "codegen/ModuleFile.h"
#include "cereal.h"
#include "codegen/CodegenVisitor.h"
#include "util/StringUtils.h"
#include <fstream>

#if 0
namespace codegen
{
    ModuleFile::ModuleFile(const CodegenInfo& i, llvm::LLVMContext& c,
                           llvm::Module* m, llvm::IRBuilder<>& b)
        : info(i), context(c), module(m), builder(b)
    {
        auto pathparts = util::stringutils::split(info.filename, '/');
        auto nameparts = util::stringutils::split(pathparts.back(), '.');
        if(nameparts.back() != "va")
        {
            nameparts.push_back("vamod");
        }
        else
        {
            nameparts.back() = "vamod";
        }
        std::for_each(nameparts.begin(), nameparts.end(),
                      [&](const std::string& p) { modulefile.append(p); });
    }

    std::vector<std::unique_ptr<Symbol>> ModuleFile::read()
    {
        std::ifstream file;
        file.exceptions(std::ios::badbit | std::ios::failbit);
        file.open(modulefile.c_str(), std::ios::binary | std::ios::in);

        ModuleFileData data;
        {
            cereal::PortableBinaryInputArchive iarchive(file);
            iarchive(data);
        }

        auto ast = std::move(data.ast);
        auto codegen = std::make_unique<CodegenVisitor>(context, module, info);
        return codegen->codegenImport(ast.get());
    }

    void ModuleFile::write(std::vector<Symbol*>&& exportSymbols)
    {
        std::ofstream file;
        file.exceptions(std::ios::badbit | std::ios::failbit);
        file.open(modulefile.c_str(), std::ios::binary | std::ios::out);

        ModuleFileData data;
        for(auto& symbol : exportSymbols)
        {
            auto& nodes = data.ast->globalNode->nodes;
            if(symbol->isFunction())
            {


            }
        }
    }
} // namespace codegen
#endif
