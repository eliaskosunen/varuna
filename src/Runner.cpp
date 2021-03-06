// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#include "Runner.h"
#include "ast/Serializer.h"
#include "codegen/Generator.h"
#include "core/Frontend.h"
#include "util/ProgramOptions.h"

Runner::Runner(int threads)
    : pool(std::make_unique<util::ThreadPool>(threads)),
      fileCache(std::make_unique<util::FileCache>())
{
}

bool Runner::run()
{
    const auto& file = util::ProgramOptions::view().inputFilename;

    if(!fileCache->addFile(file))
    {
        util::logger->error("Failed to add file '{}'", file);
        return false;
    }
    util::logger->trace("Added file to cache: '{}'", file);

    std::vector<std::future<bool>> results;
    for(const auto& f : fileCache->getFiles())
    {
        results.push_back(runFile(f).get());
    }

    for(auto&& res : results)
    {
        auto r = res.get();
        if(!r)
        {
            return false;
        }
    }

    return true;
}

std::future<std::future<bool>> Runner::runFile(std::shared_ptr<util::File> f)
{
    assert(f);
    auto file = f;
    return pool->push([this, file](int) {
        util::logger->info("Running file: '{}'", file->getFilename());

        auto ast = frontend<core::Frontend>(file);
        if(!ast)
        {
            return failedTask();
        }

        util::logger->debug("Shutting down frontend, launching code generator");

        if(util::ProgramOptions::view().output == util::EMIT_AST)
        {
            util::logger->info("File '{}' compiled successfully",
                               ast->file->getFilename());

            ast::Serializer s(ast);
            s.run(*util::loggerBasic.get(), spdlog::level::warn,
                  ast::Serializer::JSON);
            return successTask();
        }

        return runCodegen(ast);
    });
}

std::future<bool> Runner::runCodegen(std::shared_ptr<ast::AST> a)
{
    assert(a);
    auto ast = a;
    return pool->push([ast](int) -> bool {
        auto c = generate<codegen::Generator>(ast);
        if(!c)
        {
            util::logger->info(
                "Code generation of file '{}' failed, terminating\n",
                ast->file->getFilename());
            return false;
        }
        util::logger->info("File '{}' compiled successfully",
                           ast->file->getFilename());
        c->write();
        return c != nullptr;
    });
}

std::future<bool> Runner::successTask()
{
    return pool->push([](int) -> bool { return true; });
}

std::future<bool> Runner::failedTask()
{
    return pool->push([](int) -> bool { return false; });
}
