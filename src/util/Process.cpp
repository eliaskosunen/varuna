// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#include "util/Process.h"
#include "util/Compatibility.h"
#include "util/Logger.h"
#include "util/ProgramInfo.h"
#include <cstring>
#include <vector>

#if VARUNA_MSVC
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#else
#include <unistd.h>
#endif

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
    return returnValue;
}

#if VARUNA_MSVC

#if VARUNA_MSVC_UNICODE
using WinString = LPWSTR;

static std::string winStringToUtf8(WinString winstr, size_t size)
{
    if(size == 0)
    {
        return {};
    }
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

static std::string winStringToUtf8(WinString winstr, size_t size)
{
    if(size == 0)
    {
        return {};
    }
    return std::string(winstr, size);
}
#endif // VARUNA_MSVC_UNICODE

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
    return getFormattedMessage(GetLastError());
}

bool Process::_spawn()
{
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    auto command = std::vector<char>(file.begin(), file.end());
    command.push_back(' ');
    command.insert(command.end(), params.begin(), params.end());
    command.push_back('\0');

    if(!CreateProcessA(nullptr, &*command.begin(), nullptr, nullptr, false, 0,
                       nullptr, nullptr, &si, &pi))
    {
        return false;
    }

    WaitForSingleObject(pi.hProcess, INFINITE);

    DWORD exitCode = 0;
    if(!GetExitCodeProcess(pi.hProcess, &exitCode))
    {
        throw std::runtime_error(fmt::format(
            "Failed to get process exit code: {}", getErrorString()));
    }
    returnValue = static_cast<int>(exitCode);

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    spawned = true;
    return true;
}
#else

#endif // VARUNA_MSVC
} // namespace util