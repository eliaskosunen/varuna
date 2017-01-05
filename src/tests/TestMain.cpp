// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#define DOCTEST_CONFIG_IMPLEMENT
#include <doctest.h>

#include "util/Logger.h"

int main(int argc, char** argv)
{
    util::initLogger();
    // spdlog::set_level(spdlog::level::trace);
    spdlog::set_level(spdlog::level::debug);

    doctest::Context context;
    context.applyCommandLine(argc, argv);

    int res = context.run();
    return res;
}
