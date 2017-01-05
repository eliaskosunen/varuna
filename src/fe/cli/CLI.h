// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#pragma once

#include "util/ProgramInfo.h"
#include <tclap.h>

namespace fe
{
namespace api
{
    class Application;
}

namespace cli
{
    class CLI
    {
        const int argc;
        char** argv;
        TCLAP::CmdLine cmd;

        void showLicense() const;

        bool setLoggingLevel(api::Application* app, const std::string& level);

    public:
        CLI(int argc_, char** argv_);

        int run();
    };
} // namespace cli
} // namespace fe
