// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#pragma once

#include "util/Compatibility.h"
#include <spdlog/fmt/fmt.h>
#include <array>
#include <stdexcept>
#include <string>

namespace util
{
namespace programinfo
{
    namespace version
    {
        enum Status : uint8_t
        {
            STABLE = 0,
            RC = 1,
            DEV = 2
        };

        constexpr uint8_t major = 0;
        constexpr uint8_t minor = 1;
        constexpr uint8_t patch = 0;
        constexpr Status status = DEV;
        constexpr uint8_t rc = 0;

        inline const std::string& toString()
        {
            static const std::string str = [&]() {
                switch(status)
                {
                case DEV:
                    return fmt::format("{}.{}.{}-dev", major, minor, patch);
                case RC:
                    return fmt::format("{}.{}.{}-rc{}", major, minor, patch,
                                       rc);
                case STABLE:
                    return fmt::format("{}.{}.{}", major, minor, patch);
                }
            }();
            return str;
        }
    }

    constexpr const char* name = "Varuna";

    inline const std::string& getIdentifier()
    {
        static const std::string str =
            fmt::format("{} (version {})", name, version::toString());
        return str;
    }

    inline const std::string& getBuildDate()
    {
        static const std::string str = __DATE__;
        return str;
    }
} // namespace programinfo
} // namespace util
