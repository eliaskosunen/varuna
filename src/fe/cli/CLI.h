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
    public:
        CLI(int argc_, char** argv_);

        int run();

    private:
        void showLicense() const;

        bool setLoggingLevel(api::Application* app, const std::string& level);

        const int argc;
        char** argv;
        TCLAP::CmdLine cmd;
    };
} // namespace cli
} // namespace fe
