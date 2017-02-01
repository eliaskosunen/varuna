// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#pragma once

#include "util/ProgramInfo.h"
#include "util/ProgramOptions.h"

class CLI final
{
public:
    CLI(int pArgc, char** pArgv);

    /// Run CLI
    int run();

private:
    void showLicense() const;
    static void showVersion();

    const int argc;
    char** argv;
};
