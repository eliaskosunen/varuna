// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#pragma once

#include "util/ProgramInfo.h"
#include "util/ProgramOptions.h"

namespace fe
{
namespace api
{
    class Application;
} // namespace api

namespace cli
{
    class CLI final
    {
    public:
        CLI(int pArgc, char** pArgv);

        int run();

    private:
        void showLicense() const;
        static void showVersion();

        const int argc;
        char** argv;
    };
} // namespace cli
} // namespace fe
