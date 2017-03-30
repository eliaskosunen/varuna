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

#if !VARUNA_WIN32
#include <spawn.h>
#include <sys/wait.h>
#include <unistd.h>

// Required for posix_spawn
extern char** environ;
#endif // VARUNA_WIN32

namespace util
{
Process::Process(std::string pFile, std::string pParams)
    : file(std::move(pFile)), params(std::move(pParams))
{
}

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

#if VARUNA_WIN32

std::string Process::getErrorString()
{
    // errorCode is set by _spawn()
    return getFormattedMessage(errorCode);
}

std::string Process::getSuccessErrorString()
{
    return getFormattedMessage(0);
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
    return std::strerror(static_cast<int>(errorCode));
}

std::string Process::getSuccessErrorString()
{
    auto errstr = std::strerror(0);
    if(!errstr)
    {
        return "";
    }
    return errstr;
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
        errorCode = static_cast<ErrorCodeType>(status);
        return false;
    }

    // Wait for it to complete
    if(waitpid(pid, &status, 0) == -1)
    {
        errorCode = static_cast<ErrorCodeType>(status);
        return false;
    }
    if(WIFEXITED(status))
    {
        returnValue = WEXITSTATUS(status);
        spawned = true;
        return true;
    }
    if(WIFSIGNALED(status))
    {
        errorCode = static_cast<ErrorCodeType>(WTERMSIG(status));
        return false;
    }
    if(WIFSTOPPED(status))
    {
        errorCode = static_cast<ErrorCodeType>(WSTOPSIG(status));
        return false;
    }
    errorCode = static_cast<ErrorCodeType>(status);
    return false;
}

#endif // VARUNA_MSVC
} // namespace util
