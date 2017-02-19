// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#include "util/Process.h"
#include "util/Compatibility.h"
#include "util/Logger.h"
#include "util/ProgramInfo.h"
#include "util/StringUtils.h"
#include <cstring>
#include <vector>

#if VARUNA_MSVC
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#else

#include <spawn.h>
#include <sys/wait.h>
#include <unistd.h>

// Required for posix_spawn
extern char** environ;
#endif // VARUNA_MSVC

namespace util
{
bool Process::spawn()
{
    if(isSpawned())
    {
        throw std::logic_error("Process has already been spawned");
    }
    return _spawn();
}

bool Process::isSpawned() const
{
    return spawned;
}

int Process::getReturnValue() const
{
    if(!isSpawned())
    {
        throw std::logic_error("Cannot get return value of unspawned process");
    }
    return returnValue;
}

#if VARUNA_MSVC

#if VARUNA_MSVC_UNICODE
using WinString = LPWSTR;

/**
 * Convert WinString (LPWSTR, UTF-16) to std::string (UTF-8)
 * @param  winstr WinString to convert
 * @param  size   Length of string
 * @return        Corresponding std::string
 */
static std::string winStringToUtf8(WinString winstr, size_t size)
{
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
using WinString = LPSTR;

/**
 * Convert WinString (LPSTR, UTF-8) to std::string (UTF-8)
 * @param  winstr WinString to convert
 * @param  size   Length of string
 * @return        Corresponding std::string
 */
static std::string winStringToUtf8(WinString winstr, size_t size)
{
    if(size == 0)
    {
        return {};
    }
    return std::string(winstr, size);
}
#endif // VARUNA_MSVC_UNICODE

/**
 * Get OS error message from error code
 * @param  msgID Error code from GetLastError()
 * @return       Formatted error message
 */
static std::string getFormattedMessage(DWORD msgID)
{
    if(msgID == 0)
    {
        return {};
    }

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

std::string Process::getErrorString()
{
    // errorCode is set by _spawn()
    return getFormattedMessage(errorCode);
}

bool Process::_spawn()
{
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    // CreateProcessA requires a C-style array
    // C++ doesn't have VLA's, so we use a std::vector instead
    // and then get a pointer to its first element
    auto command = std::vector<char>(file.begin(), file.end());
    command.push_back(' ');
    command.insert(command.end(), params.begin(), params.end());
    command.push_back('\0');

    if(!CreateProcessA(nullptr, &command[0], nullptr, nullptr, false, 0,
                       nullptr, nullptr, &si, &pi))
    {
        errorCode = GetLastError();
        return false;
    }

    // Wait for it to execute
    WaitForSingleObject(pi.hProcess, INFINITE);

    DWORD exitCode = 0;
    if(!GetExitCodeProcess(pi.hProcess, &exitCode))
    {
        errorCode = GetLastError();
        throw std::runtime_error(fmt::format(
            "Failed to get process exit code: {}", getErrorString()));
    }
    returnValue = static_cast<int>(exitCode);

    // Win32 API doesn't know of RAII
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    spawned = true;
    return true;
}

#else

std::string Process::getErrorString()
{
    return std::string(std::strerror(errorCode));
}

bool Process::_spawn()
{
    // Oh god I love C-style arrays
    //
    // Vector that contains argv
    // argv[0] is the executable name
    std::vector<std::string> vec{file};
    // Split the argument string by spaces
    // and push them to the vector
    util::stringutils::split(params, ' ', vec);
    // Convert vector of std::string to a vector of C-strings
    std::vector<char*> argvVec;
    // 1 extra slot is for the null terminator
    argvVec.reserve(vec.size() + 1);
    for(auto& a : vec)
    {
        // Push the pointer to the first character
        argvVec.push_back(&a[0]);
    }
    // argv[argc] has to be null so argc can be calculated
    // We were talking about this null terminator earlier
    argvVec.push_back(static_cast<char*>(nullptr));
    // Get the actual argv from pointer to the first element of argvVec
    char** argv = &argvVec[0];

    // Flush the standard streams before spawning a new process
    std::fflush(nullptr);
    pid_t pid;
    auto status =
        posix_spawnp(&pid, file.c_str(), nullptr, nullptr, argv, environ);
    if(status != 0)
    {
        errorCode = status;
        return false;
    }

    // Wait for it to complete
    if(waitpid(pid, &status, 0) == -1)
    {
        errorCode = status;
        return false;
    }
    returnValue = status;
    spawned = true;
    return true;
}

#endif // VARUNA_MSVC
} // namespace util
