// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#pragma once

#include "util/Compatibility.h"
#include <spdlog.h>
#include <array>
#include <cstring>
#include <stdexcept>
#include <string>

namespace util
{
namespace programinfo
{
    namespace version
    {
        // Version type
        enum class Status : uint8_t
        {
            STABLE = 0, ///< Stable release
            RC = 1,     ///< Release candidate
            DEV = 2     ///< Development version
        };

        constexpr uint8_t major = 0; ///< Major version number: X.x.x
        constexpr uint8_t minor = 1; ///< Minor version number: x.X.x
        constexpr uint8_t patch = 0; ///< Patch number: x.x.X
        constexpr uint8_t rc = 2;    ///< Release candidate number: x.x.x-rcX
        constexpr Status status = Status::DEV; ///< Version type

        /**
         * Format a version string
         * \return Formatted version
         */
        const std::string& toString();
    } // namespace version

    /// Program name
    constexpr const char* name = "Varuna";
    /// Program identifier
    const std::string& getIdentifier();
    /// Program build date
    const std::string& getBuildDate();
} // namespace programinfo
} // namespace util
