// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#include "CLI.h"
#include "util/Logger.h"
#include <utf8.h>
#include <cstdlib>
#include <stdexcept>

/// Clean up loggers
static void cleanup()
{
    util::dropLogger();
}

/**
 * Log an exception
 * @param msg Exception message
 */
static void logException(const std::string& msg)
{
    if(util::logger)
    {
        // util::logger is available, prioritize using it
        util::logger->critical(
            "An exception occured during program execution.");
        util::logger->info("Exception message: '{}'", msg);
        util::logger->info("Program will be terminated.");
    }
    else if(util::loggerBasic)
    {
        // util::loggerBasic is available, use it if util::logger isn't
        util::loggerBasic->critical(
            "An exception occured during program execution.");
        util::loggerBasic->info("Exception message: '{}'", msg);
        util::loggerBasic->info("Program will be terminated.");
    }
    else
    {
        // Fallback on C stdio
        std::fprintf(stderr,
                     "An exception occured during program execution.\n");
        std::fprintf(stderr, "Exception message: '%s'\n", msg.c_str());
        std::fprintf(stderr, "Program will be terminated.\n");
        std::fprintf(stderr, "No loggers available for message delivery.\n");
        std::fflush(stderr);
    }
}

/**
 * Log an exception with an additional message
 * @param msg           Exception message
 * @param additionalMsg Additional message
 */
static void logException(const std::string& msg,
                         const std::string& additionalMsg)
{
    if(util::logger)
    {
        // util::logger is available, prioritize using it
        util::logger->critical(
            "An exception occured during program execution.");
        util::logger->info("Exception message: '{}'", msg);
        util::logger->info("Additional message: '{}'", additionalMsg);
        util::logger->info("Program will be terminated.");
    }
    else if(util::loggerBasic)
    {
        // util::loggerBasic is available, use it if util::logger isn't
        util::loggerBasic->critical(
            "An exception occured during program execution.");
        util::loggerBasic->info("Exception message: '{}'", msg);
        util::loggerBasic->info("Additional message: '{}'", additionalMsg);
        util::loggerBasic->info("Program will be terminated.");
    }
    else
    {
        // Fallback on C stdio
        std::fprintf(stderr,
                     "An exception occured during program execution.\n");
        std::fprintf(stderr, "Exception message: '%s'\n", msg.c_str());
        std::fprintf(stderr, "Additional message: '%s'\n",
                     additionalMsg.c_str());
        std::fprintf(stderr, "Program will be terminated.\n");
        std::fprintf(stderr, "No loggers available for message delivery.\n");
        std::fflush(stderr);
    }
}

#define BREAK_AFTER_EXECUTION 0

#if BREAK_AFTER_EXECUTION
#define MAIN_RETURN(expr)                                                      \
    {                                                                          \
        const auto val = expr;                                                 \
        std::fprintf(stderr, "Press RETURN to continue...");                   \
        const auto c = std::getchar();                                         \
        return val;                                                            \
    }
#else
#define MAIN_RETURN(expr) return expr;
#endif

int main(int argc, char** argv)
{
    try
    {
        // Cleanup in the end
        const int atexitResult = std::atexit(cleanup);
        if(atexitResult != 0)
        {
            throw std::runtime_error("Failed to register cleanup function\n");
        }

        // Initialize loggers
        util::initLogger();

        // Run the CLI
        CLI commandLineInterface(argc, argv);
        MAIN_RETURN(commandLineInterface.run());
    }

    // Handle various exceptions
    catch(const spdlog::spdlog_ex& e)
    {
        logException(e.what());
    }

    catch(const utf8::invalid_code_point& e)
    {
        logException(e.what(),
                     "Invalid code point: " + std::to_string(e.code_point()));
    }
    catch(const utf8::invalid_utf8& e)
    {
        logException(e.what(),
                     "Invalid utf8: " +
                         std::to_string(static_cast<uint32_t>(e.utf8_octet())));
    }
    catch(const utf8::not_enough_room& e)
    {
        logException(e.what());
    }
    catch(const utf8::exception& e)
    {
        logException(e.what());
    }

    catch(const std::logic_error& e)
    {
        logException(e.what(), "Logic error");
    }
    catch(const std::runtime_error& e)
    {
        logException(e.what(), "Runtime error");
    }
    catch(const std::exception& e)
    {
        logException(e.what());
    }

    // Return -1 on exceptional circumstances
    MAIN_RETURN(-1);
}
