// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#include "codegen/Generator.h"
#include "util/ProgramOptions.h"

namespace codegen
{
Generator::Generator(std::shared_ptr<ast::AST> t) : ast(t), c{nullptr}
{
    // Create CodegenInfo based on ProgramOptions
    const auto& opt = util::viewProgramOptions().getOptLevel();
    CodegenInfo cinfo(t->file, std::get<0>(opt), std::get<1>(opt),
                      util::viewProgramOptions().emitDebug);

    // Create code generator
    c = std::make_unique<GeneratorClass>(t, cinfo);
}

bool Generator::run()
{
    util::logger->debug("Starting code generation");
    // Run code generator
    if(!c->run())
    {
        util::logger->debug("Code generation failed");
        return false;
    }
    util::logger->debug("Code generation finished\n");
    return true;
}

std::unique_ptr<Generator::GeneratorClass> Generator::get()
{
    return std::move(c);
}
}
