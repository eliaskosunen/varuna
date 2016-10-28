//
// Copyright(c) 2015 Gabi Melman.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#pragma once

///////////////////////////////////////////////////////////////////////////////
//
// Edit this file to squeeze more performance, and to customize supported features
//
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// Under Linux, the much faster CLOCK_REALTIME_COARSE clock can be used.
// This clock is less accurate - can be off by dozens of millis - depending on the kernel HZ.
// Uncomment to use it instead of the regular clock.
//
// #define SPDLOG_CLOCK_COARSE
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// Uncomment if date/time logging is not needed and never appear in the log pattern.
// This will prevent spdlog from quering the clock on each log call.
//
// WARNING: If the log pattern contains any date/time while this flag is on, the result is undefined.
//          You must set new pattern(spdlog::set_pattern(..") without any date/time in it
//
// #define SPDLOG_NO_DATETIME
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// Uncomment if thread id logging is not needed (i.e. no %t in the log pattern).
// This will prevent spdlog from quering the thread id on each log call.
//
// WARNING: If the log pattern contains thread id (i.e, %t) while this flag is on, the result is undefined.
//
#define SPDLOG_NO_THREAD_ID
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// Uncomment if logger name logging is not needed.
// This will prevent spdlog from copying the logger name  on each log call.
//
#define SPDLOG_NO_NAME
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Uncomment to enable the SPDLOG_DEBUG/SPDLOG_TRACE macros.
//
// #define SPDLOG_DEBUG_ON
// #define SPDLOG_TRACE_ON
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// Uncomment to avoid locking in the registry operations (spdlog::get(), spdlog::drop() spdlog::register()).
// Use only if your code never modifes concurrently the registry.
// Note that upon creating a logger the registry is modified by spdlog..
//
// #define SPDLOG_NO_REGISTRY_MUTEX
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// Uncomment to avoid spdlog's usage of atomic log levels
// Use only if your code never modifies a logger's log levels concurrently by different threads.
//
// #define SPDLOG_NO_ATOMIC_LEVELS
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Uncomment to enable usage of wchar_t for file names on Windows.
//
// #define SPDLOG_WCHAR_FILENAMES
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Uncomment to override default eol ("\n" or "\r\n" under Linux/Windows)
//
// #define SPDLOG_EOL ";-)\n"
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// Uncomment to use your own copy of the fmt library instead of spdlog's copy.
// In this case spdlog will try to include <fmt/format.h> so set your -I flag accordingly.
//
// #define SPDLOG_FMT_EXTERNAL
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// Uncomment to enable syslog (disabled by default)
//
// #define SPDLOG_ENABLE_SYSLOG
///////////////////////////////////////////////////////////////////////////////
