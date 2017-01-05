// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#include "fe/api/App.h"
#include "core/codegen/Codegen.h"
#include "core/lexer/Lexer.h"
#include "core/parser/Parser.h"

namespace fe
{
namespace api
{
    void Application::setLoggingLevel(fe::api::Application::LoggingLevel l)
    {
        spdlog::set_level(static_cast<spdlog::level::level_enum>(l));
    }

    bool Application::addFile(const std::string& file)
    {
        if(!fileCache->addFile(file))
        {
            util::logger->error("Failed to add file '{}'", file);
            return false;
        }
        util::logger->trace("Added file to cache: '{}'", file);
        return true;
    }

    std::future<bool> Application::runFile(const std::string& file)
    {
        return pool->push([&](int) {
            util::logger->info("Running file: '{}'", file);

            auto f = fileCache->getFile(file);
            if(!f)
            {
                util::logger->error("No file '{}' found from cache", file);
                return false;
            }

            util::logger->debug("Starting lexer");
            core::lexer::Lexer l(std::move(f->content), file);
            auto tokens = l.run();
            if(l.getError())
            {
                util::logger->debug("Lexing failed");
                util::logger->info("Lexing of file '{}' failed, terminating\n",
                                   file);
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
            core::codegen::CodegenInfo cinfo{ast->file, 3, 0};
            core::codegen::Codegen codegen(std::move(ast), std::move(cinfo));
            if(!codegen.run())
            {
                util::logger->debug("Code generation failed");
                util::logger->info(
                    "Code generation of file '{}' failed, terminating\n", file);
                return false;
            }
            util::logger->debug("Code generation finished\n");

            util::logger->info("File '{}' compiled successfully", file);

            return true;
        });
    }

    bool Application::runFiles(std::vector<std::string> files)
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
        return runFiles(fileCache->getFileList());
    }

    bool Application::execute(std::vector<std::string> files)
    {
        for(const auto& file : files)
        {
            if(!addFile(file))
            {
                return false;
            }
        }

        return runFiles(std::move(files));
    }
}
}
