// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#include "fe/app/App.h"
#include "core/codegen/Codegen.h"
#include "core/lexer/Lexer.h"
#include "core/parser/Parser.h"
#include "util/ProgramOptions.h"

namespace fe
{
namespace api
{
    bool Application::addFile(const std::string& filename)
    {
        if(!fileCache->addFile(filename))
        {
            util::logger->error("Failed to add file '{}'", filename);
            return false;
        }
        util::logger->trace("Added file to cache: '{}'", filename);
        return true;
    }

    std::future<bool> Application::runFile(std::shared_ptr<util::File> f)
    {
        assert(f.get());

        // Get copy to prevent lifetime problems
        // f is shared_ptr, cheap to copy
        auto file = f;

        return pool->push([file](int) {
            assert(file.get());

            util::logger->info("Running file: '{}'", file->getFilename());

            util::logger->debug("Starting lexer");
            core::lexer::Lexer l(file.get());
            auto tokens = l.run();
            if(l.getError())
            {
                util::logger->debug("Lexing failed");
                util::logger->info("Lexing of file '{}' failed, terminating\n",
                                   file->getFilename());
                return false;
            }
            util::logger->debug("Lexing finished\n");

            util::logger->debug("Starting parser");
            core::parser::Parser p(tokens);
            p.run();
            if(p.getError())
            {
                util::logger->debug("Parsing failed");
                util::logger->info("Parsing of file '{}' failed, terminating\n",
                                   tokens[0].loc.file);
                return false;
            }
            auto ast = p.retrieveAST();
            util::logger->debug("Parsing finished\n");

            core::ast::DumpASTVisitor::dump<core::ast::ASTBlockStatement>(
                ast->globalNode.get());

            util::logger->debug("Starting code generation");
            const auto& opt = util::getProgramOptions().getOptLevel();
            core::codegen::CodegenInfo cinfo{ast->file, std::get<0>(opt),
                                             std::get<1>(opt)};
            core::codegen::Codegen codegen(std::move(ast), cinfo);
            if(!codegen.run())
            {
                util::logger->debug("Code generation failed");
                util::logger->info(
                    "Code generation of file '{}' failed, terminating\n",
                    file->getFilename());
                return false;
            }
            util::logger->debug("Code generation finished\n");

            util::logger->info("File '{}' compiled successfully",
                               file->getFilename());

            return true;
        });
    }

    bool Application::runFiles(std::vector<std::shared_ptr<util::File>> files)
    {
        std::vector<std::future<bool>> results;
        for(const auto& file : files)
        {
            results.emplace_back(runFile(file));
        }

        for(auto&& result : results)
        {
            if(!result.get())
            {
                return false;
            }
        }
        return true;
    }

    bool Application::runAll()
    {
        return runFiles(fileCache->getFiles());
    }

    bool Application::execute()
    {
        const auto& files = util::getProgramOptions().inputFilenames;

        for(const auto& file : files)
        {
            if(!addFile(file))
            {
                return false;
            }
        };

        auto filelist = fileCache->getFilesByNames(files);
        return runFiles(std::move(filelist));
    }
} // namespace api
} // namespace fe
