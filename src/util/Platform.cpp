// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#include "util/Platform.h"
#include <fmt.h>
#include <climits>
#include <cstdlib>
#include <stdexcept>

#if VARUNA_WIN32
#include <PathCch.h>
#include <direct.h>
#define GETCWD _getcwd
#else
#include <unistd.h>
#define GETCWD getcwd
#endif

#if VARUNA_LINUX
#include "util/BinReloc.h"
#elif VARUNA_APPLE
#include <libgen.h>
#include <mach-o/dyld.h>
#endif

namespace util
{
std::string getCurrentDirectory()
{
    char buf[FILENAME_MAX];
    if(GETCWD(buf, sizeof(buf)) != nullptr)
    {
        return std::string(buf);
    }

    char* errstr = std::strerror(errno);
    if(!errstr)
    {
        throw std::runtime_error(fmt::format(
            "Unable to get current directory: Unknown errno: {}", errno));
    }
    throw std::runtime_error(
        fmt::format("Unable to get current directory: {}", errstr));
}

std::string getExecDirectory()
{
#if VARUNA_WIN32
    WinChar buf[FILENAME_MAX];
    if(!GetModuleFileName(nullptr, buf, FILENAME_MAX))
    {
        throw std::runtime_error(
            fmt::format("Unable to get current directory: {}",
                        getFormattedMessage(GetLastError())));
    }
    auto err = PathCchRemoveFileSpec(buf, FILENAME_MAX);
    if(FAILED(err))
    {
        throw std::runtime_error(fmt::format(
            "Unable to get current directory: {}", getFormattedMessage(err)));
    }
    return winStringToUtf8(buf, 0, true);
#elif VARUNA_APPLE
    char path[FILENAME_MAX];
    uint32_t size = sizeof(path);
    if(_NSGetExecutablePath(path, &size) != 0)
    {
        throw std::length_error(
            "Unable to get current directory: Not enough space");
    }
    char buf[FILENAME_MAX];
    if(!realpath(path, buf))
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
    auto str = dirname(buf);
    if(!str)
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
    return str;
#else
    BinReloc br;
    return br.getExecDirectory(getCurrentDirectory());
#endif
}

#if VARUNA_WIN32
#if VARUNA_MSVC_UNICODE
/**
 * Convert WinString (LPWSTR, UTF-16) to std::string (UTF-8)
 * \param  winstr WinString to convert
 * \param  size   Length of string
 * \return        Corresponding std::string
 */
std::string winStringToUtf8(WinString winstr, size_t size, bool countLen)
{
    if(countLen)
    {
        size = std::wcslen(winstr);
    }
    if(size == 0)
    {
        return {};
    }
    // Not the best APIs around
    auto sizeNeeded =
        WideCharToMultiByte(CP_UTF8, 0, &winstr[0], static_cast<int>(size),
                            nullptr, 0, nullptr, nullptr);
    std::string strTo(sizeNeeded, 0);
    WideCharToMultiByte(CP_UTF8, 0, &winstr[0], static_cast<int>(size),
                        &strTo[0], sizeNeeded, nullptr, nullptr);
    return strTo;
}
#else
/**
 * Convert WinString (LPSTR, UTF-8) to std::string (UTF-8)
 * \param  winstr WinString to convert
 * \param  size   Length of string
 * \return        Corresponding std::string
 */
std::string winStringToUtf8(WinString winstr, size_t size, bool countLen)
{
    if(countLen)
    {
        size = std::strlen(winstr);
    }
    if(size == 0)
    {
        return {};
    }
    return std::string(winstr, size);
}
#endif // VARUNA_MSVC_UNICODE

/**
 * Get OS error message from error code
 * \param  msgID Error code from GetLastError()
 * \return       Formatted error message
 */
std::string getFormattedMessage(DWORD msgID)
{
    WinString buf = nullptr;
    auto size = FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr, msgID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (WinString)&buf, 0, nullptr);

    auto msg = winStringToUtf8(buf, size);
    LocalFree(buf);

    // Remove possible line breaks from the end
    if(msg.back() == '\n')
    {
        msg.pop_back();
    }
    if(msg.back() == '\r')
    {
        msg.pop_back();
    }
    return msg;
}
#endif // VARUNA_WIN32
} // namespace util
