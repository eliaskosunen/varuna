// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#include "CLI.h"
#include "util/Logger.h"
#include <utf8.h>
#include <cstdlib>
#include <stdexcept>

static void cleanup()
{
    util::dropLogger();
}

static void logException(const std::string& msg)
{
    if(util::logger)
    {
        util::logger->critical(
            "An exception occured during program execution.");
        util::logger->info("Exception message: '{}'", msg);
        util::logger->info("Program will be terminated.");
    }
    else if(util::loggerBasic)
    {
        util::loggerBasic->critical(
            "An exception occured during program execution.");
        util::loggerBasic->info("Exception message: '{}'", msg);
        util::loggerBasic->info("Program will be terminated.");
    }
    else
    {
        fprintf(stderr, "An exception occured during program execution.\n");
        fprintf(stderr, "Exception message: '%s'\n", msg.c_str());
        fprintf(stderr, "Program will be terminated.\n");
        fprintf(stderr, "No loggers available for message delivery.\n");
        fflush(stderr);
    }
}

static void logException(const std::string& msg,
                         const std::string& additionalMsg)
{
    if(util::logger)
    {
        util::logger->critical(
            "An exception occured during program execution.");
        util::logger->info("Exception message: '{}'", msg);
        util::logger->info("Additional message: '{}'", additionalMsg);
        util::logger->info("Program will be terminated.");
    }
    else if(util::loggerBasic)
    {
        util::loggerBasic->critical(
            "An exception occured during program execution.");
        util::loggerBasic->info("Exception message: '{}'", msg);
        util::loggerBasic->info("Additional message: '{}'", additionalMsg);
        util::loggerBasic->info("Program will be terminated.");
    }
    else
    {
        fprintf(stderr, "An exception occured during program execution.\n");
        fprintf(stderr, "Exception message: '%s'\n", msg.c_str());
        fprintf(stderr, "Additional message: '%s'\n", additionalMsg.c_str());
        fprintf(stderr, "Program will be terminated.\n");
        fprintf(stderr, "No loggers available for message delivery.\n");
        fflush(stderr);
    }
}

int main(int argc, char** argv)
{
    try
    {
        const int atexitResult = std::atexit(cleanup);
        if(atexitResult != 0)
        {
            throw std::runtime_error("Failed to register cleanup function\n");
        }

        util::initLogger();

        CLI commandLineInterface(argc, argv);
        return commandLineInterface.run();
    }
    catch(const spdlog::spdlog_ex& e)
    {
        fprintf(stderr, "An exception occured during program execution.\n");
        fprintf(stderr, "Application logger threw an exception.\n");
        fprintf(stderr, "Exception message: '%s'\n", e.what());
        fprintf(stderr, "Program will be terminated.\n");
        fflush(stderr);
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
    return -1;
}
