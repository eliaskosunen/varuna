// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#pragma once

#define VALUE_TO_STRING(x) #x
#define VALUE(x) VALUE_TO_STRING(x)

#ifdef _MSC_VER
#define VARUNA_MSVC 1
#else
#define VARUNA_MSVC 0
#endif

#if VARUNA_MSVC
#if defined(UNICODE) || defined(_UNICODE)
#define VARUNA_MSVC_UNICODE 1
#else
#define VARUNA_MSVC_UNICODE 0
#endif // defined (UNICODE)
#else
#define VARUNA_MSVC_UNICODE 0
#endif // VARUNA_MSVC

#if defined(_WIN32) || defined(WIN32)
#define VARUNA_WIN32 1
#else
#define VARUNA_WIN32 0
#endif // VARUNA_WIN32

#if defined(__linux__) || defined(linux) || defined(__linux) || defined(_linux)
#define VARUNA_LINUX 1
#else
#define VARUNA_LINUX 0
#endif

#ifdef __APPLE__
#include "TargetConditionals.h"
#define VARUNA_APPLE 1
#else
#define VARUNA_APPLE 0
#endif

#include <string>

#if VARUNA_WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#undef min
#undef max
#endif

namespace util
{
std::string getCurrentDirectory();

#if VARUNA_WIN32
#if VARUNA_MSVC_UNICODE
using WinString = LPWSTR;
using WinChar = wchar_t;

/**
 * Convert WinString (LPWSTR, UTF-16) to std::string (UTF-8)
 * @param  winstr WinString to convert
 * @param  size   Length of string
 * @return        Corresponding std::string
 */
std::string winStringToUtf8(WinString winstr, size_t size,
                            bool countLen = false);
#else
using WinString = LPSTR;
using WinChar = char;

/**
 * Convert WinString (LPSTR, UTF-8) to std::string (UTF-8)
 * @param  winstr WinString to convert
 * @param  size   Length of string
 * @return        Corresponding std::string
 */
std::string winStringToUtf8(WinString winstr, size_t size,
                            bool countLen = false);
#endif // VARUNA_MSVC_UNICODE

/**
 * Get OS error message from error code
 * @param  msgID Error code from GetLastError()
 * @return       Formatted error message
 */
std::string getFormattedMessage(DWORD msgID);
#endif // VARUNA_WIN32

std::string getExecDirectory();
} // namespace util
