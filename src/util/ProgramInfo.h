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

#if VARUNA_MSVC
#define WIN32_LEAN_AND_MEAN
#include <direct.h>
#define GETCWD _getcwd
#else
#include <unistd.h>
#define GETCWD getcwd
#endif

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
        constexpr uint8_t rc = 0;    ///< Release candidate number: x.x.x-rcX
        constexpr Status status = Status::DEV; ///< Version type

        /**
         * Format a version string
         * @return Formatted version
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

inline std::string getCurrentDirectory()
{
    char buf[FILENAME_MAX];
    if(GETCWD(buf, sizeof(buf)) != 0)
    {
        return std::string(buf);
    }

    switch(errno)
    {
    case EACCES:
        throw std::runtime_error(
            "util::getCurrentDirectory() failed: Access denied (EACCES)");
    case ENOMEM:
        throw std::runtime_error(
            "util::getCurrentDirectory() failed: No memory (ENOMEM)");
    default:
    {
        char* errstr = std::strerror(errno);
        if(!errstr)
        {
            throw std::runtime_error(fmt::format(
                "Unable to get current directory: Unknown errno: {}", errno));
        }
        throw std::runtime_error(
            fmt::format("Unable to get current directory: {}", errstr));
    }
    }
}
} // namespace util

#undef GETCWD
