// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#pragma once

#include <binreloc.h>
#include <fmt.h>
#include <stdexcept>
#include <string>

namespace util
{
/// C++ wrapper around C binreloc.
/// Use only if not on Windows or Mac
class BinReloc
{
    std::string brInitErrorToString(BrInitError error) const;

public:
    BinReloc();

    /**
     * Get executable filename.
     * Example: /usr/bin/varuna
     * \param  fallback Fallback name if no name could be get
     * \throw  std::runtime_error If name getting failed and no fallback was
     * given \return          Executable filename
     */
    std::string getExecFilename(const std::string& fallback = "") const;

    /**
     * Get executable directory.
     * Example: /usr/bin
     * \param  fallback Fallback name if no name could be get
     * \throw  std::runtime_error If name getting failed and no fallback was
     * given \return          Executable directory
     */
    std::string getExecDirectory(const std::string& fallback = "") const;

    /**
     * Get executable prefix.
     * Example: /usr
     * \param  fallback Fallback name if no name could be get
     * \throw s td::runtime_error If name getting failed and no fallback was
     * given \return          Executable prefix
     */
    std::string getExecPrefix(const std::string& fallback = "") const;

    /**
     * Get the directory name out of a path.
     * Example: /usr/bin/varuna -> /usr/bin
     * \param  path Path to extract directory from
     * \throw  std::runtime_error On error
     * \return      Directory
     */
    std::string dirname(const std::string& path) const;
};

inline BinReloc::BinReloc()
{
    BrInitError err;
    if(!br_init_lib(&err))
    {
        throw std::runtime_error(fmt::format(
            "Failed to initialize BinReloc: {}", brInitErrorToString(err)));
    }
}

inline std::string BinReloc::getExecFilename(const std::string& fallback) const
{
    auto name = br_find_exe(fallback.empty() ? nullptr : fallback.c_str());
    if(!name)
    {
        throw std::runtime_error(
            "Failed to get executable filename: No fallback given");
    }
    auto str = std::string(name);
    free(name);
    return str;
}

inline std::string BinReloc::getExecDirectory(const std::string& fallback) const
{
    auto dir = br_find_exe_dir(fallback.empty() ? nullptr : fallback.c_str());
    if(!dir)
    {
        throw std::runtime_error(
            "Failed to get executable directory: No fallback given");
    }
    auto str = std::string(dir);
    free(dir);
    return str;
}

inline std::string BinReloc::getExecPrefix(const std::string& fallback) const
{
    auto prefix = br_find_prefix(fallback.empty() ? nullptr : fallback.c_str());
    if(!prefix)
    {
        throw std::runtime_error(
            "Failed to get executable prefix: No fallback given");
    }
    auto str = std::string(prefix);
    free(prefix);
    return str;
}

inline std::string BinReloc::dirname(const std::string& path) const
{
    auto dir = br_dirname(path.c_str());
    if(!dir)
    {
        throw std::runtime_error("Failed to get directory from path");
    }
    auto str = std::string(dir);
    free(dir);
    return str;
}

inline std::string BinReloc::brInitErrorToString(BrInitError error) const
{
    switch(error)
    {
    case BR_INIT_ERROR_NOMEM:
        return "Memory allocation failed";
    case BR_INIT_ERROR_OPEN_MAPS:
        return fmt::format("Failed to open /proc/self/maps: {}",
                           std::strerror(errno));
    case BR_INIT_ERROR_READ_MAPS:
        return fmt::format("Failed to read /proc/self/maps: {}",
                           std::strerror(errno));
    case BR_INIT_ERROR_INVALID_MAPS:
        return "Invalid /proc/self/maps";
    case BR_INIT_ERROR_DISABLED:
        return "BinReloc is disabled";
    }
    return "";
}
} // namespace util
