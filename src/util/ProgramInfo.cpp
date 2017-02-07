// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#include "util/ProgramInfo.h"
#include <spdlog.h>

namespace util
{
namespace programinfo
{
    const std::string& version::toString()
    {
        static const std::string str = [&]() {
            switch(status)
            {
            case Status::DEV:
                return fmt::format("{}.{}.{}-dev", major, minor, patch);
            case Status::RC:
                return fmt::format("{}.{}.{}-rc{}", major, minor, patch, rc);
            case Status::STABLE:
                return fmt::format("{}.{}.{}", major, minor, patch);
            }
        }();
        return str;
    }

    const std::string& getIdentifier()
    {
        static const std::string str =
            fmt::format("{} (version {})", name, version::toString());
        return str;
    }

    const std::string& getBuildDate()
    {
        static const std::string str = __DATE__;
        return str;
    }
} // namespace programinfo
} // namespace util
