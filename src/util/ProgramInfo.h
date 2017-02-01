// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#pragma once

#include "util/Compatibility.h"
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

        const std::string& toString();
    } // namespace version

    constexpr const char* name = "Varuna";

    const std::string& getIdentifier();
    const std::string& getBuildDate();
} // namespace programinfo
} // namespace util
