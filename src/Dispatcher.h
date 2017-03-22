// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#pragma once

#include "ast/AST.h"
#include "ast/DumpVisitor.h"
#include "codegen/Codegen.h"
#include "util/File.h"
#include "util/ProgramOptions.h"

/**
 * Run a frontend, such as `core`
 * @param  file File to run frontend on
 * @return      AST generated from file
 */
template <class Frontend>
inline std::shared_ptr<ast::AST> frontend(std::shared_ptr<util::File> file)
{
    assert(file);

    auto fe = std::make_unique<Frontend>(file);
    util::logger->debug("Starting frontend: {}", fe->getIdentifier());

    auto ast = fe->run();
    if(!ast)
    {
        util::logger->debug("Frontend '{}' finished unsuccessfully",
                            fe->getIdentifier());
        return nullptr;
    }
    util::logger->debug("Frontend '{}' finished successfully",
                        fe->getIdentifier());
    return ast;
}

/**
 * Run code generation on AST
 * @param  ast AST to generate code of
 * @return     Codegen class
 */
template <class Generator>
inline std::unique_ptr<typename Generator::GeneratorClass>
generate(std::shared_ptr<ast::AST> ast)
{
    assert(ast);

    auto gen = std::make_unique<Generator>(ast);
    util::logger->debug("Starting generator: {}", gen->getIdentifier());

    if(!gen->run())
    {
        util::logger->debug("Generator '{}' finished unsuccessfully",
                            gen->getIdentifier());
        return nullptr;
    }
    util::logger->debug("Generator '{}' finished successfully",
                        gen->getIdentifier());
    return gen->get();
}
