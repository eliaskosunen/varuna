// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#include "Runner.h"
#include "codegen/Generator.h"
#include "core/Frontend.h"

Runner::Runner(int threads)
    : pool(std::make_unique<util::ThreadPool>(threads)),
      fileCache(std::make_unique<util::FileCache>())
{
}

bool Runner::run()
{
    const auto& files = util::viewProgramOptions().inputFilenames;

    for(const auto& file : files)
    {
        if(!fileCache->addFile(file))
        {
            util::logger->error("Failed to add file '{}'", file);
            return false;
        }
        util::logger->trace("Added file to cache: '{}'", file);
    }

    std::vector<std::future<std::shared_ptr<ast::AST>>> frontendResults;
    for(const auto& f : fileCache->getFiles())
    {
        frontendResults.push_back(runFrontend(f));
    }

    std::vector<std::future<std::unique_ptr<codegen::Codegen>>> codegenResults;
    for(auto&& fres : frontendResults)
    {
        auto r = fres.get();
        if(!r)
        {
            return false;
        }
        codegenResults.push_back(runCodegen(std::move(r)));
    }

    for(auto&& cres : codegenResults)
    {
        auto r = cres.get();
        if(!r)
        {
            return false;
        }
    }

    return true;
}

std::future<std::shared_ptr<ast::AST>>
Runner::runFrontend(std::shared_ptr<util::File> f)
{
    auto file = f;
    return pool->push([file](int) {
        util::logger->info("Running file: '{}'", file->getFilename());
        auto r = frontend<core::Frontend>(file);
        return std::shared_ptr<ast::AST>{std::move(r)};
    });
}
std::future<std::unique_ptr<codegen::Codegen>>
Runner::runCodegen(std::shared_ptr<ast::AST> t)
{
    auto ast = t;
    return pool->push([ast](int) {
        auto c = generate<codegen::Generator>(ast);
        if(!c)
        {
            util::logger->info(
                "Code generation of file '{}' failed, terminating\n",
                ast->file->getFilename());
            return c;
        }
        util::logger->info("File '{}' compiled successfully",
                           ast->file->getFilename());
        c->write();
        return c;
    });
}
