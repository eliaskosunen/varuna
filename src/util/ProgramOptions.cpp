// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#include "util/ProgramOptions.h"

namespace util
{
ProgramOptions ProgramOptions::instance;

std::pair<uint8_t, uint8_t> ProgramOptions::getOptLevel() const
{
    uint8_t o = 0;
    uint8_t s = 0;
    switch(optLevel)
    {
    case OPT_O0:
        break;
    case OPT_O1:
        o = 1;
        break;
    case OPT_O2:
        o = 2;
        break;
    case OPT_O3:
        o = 3;
        break;
    case OPT_Os:
        o = 2;
        s = 1;
        break;
    case OPT_Oz:
        o = 2;
        s = 2;
        break;
    }

    return {o, s};
}

std::string ProgramOptions::optLevelToString() const
{
    uint8_t o, s;
    std::tie(o, s) = getOptLevel();
    if(o == 0 && s == 0)
    {
        return "-O0";
    }
    if(s == 0)
    {
        return fmt::format("-O{}", o);
    }
    if(s == 1)
    {
        return "-Os";
    }
    return "-Oz";
}
}
