// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#include "fe/cli/CLI.h"
#include "util/Logger.h"
#include <utf8.h>
#include <cstdlib>
#include <iostream>
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
        std::cerr << "An exception occured during program execution.\n"
                  << "Exception message: '" << msg << "'\n"
                  << "Program will be terminated.\n"
                  << "No loggers available for message delivery." << std::endl;
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
        std::cerr << "An exception occured during program execution.\n"
                  << "Exception message: '" << msg << "'\n"
                  << "Additional message: '" << additionalMsg << "'\n"
                  << "Program will be terminated.\n"
                  << "No loggers available for message delivery." << std::endl;
    }
}

int main(int argc, char** argv)
{
    try
    {
        const int atexitResult = std::atexit(cleanup);
        if(atexitResult != 0)
        {
            std::cerr << "Failed to register cleanup function\n";
            return -1;
        }

        util::initLogger();

        fe::cli::CLI commandLineInterface(argc, argv);
        return commandLineInterface.run();
    }
    catch(const spdlog::spdlog_ex& e)
    {
        std::cerr << "An exception occured during program execution.\n"
                  << "Application logger threw an exception.\n"
                  << "Exception message: '" << e.what() << "'\n"
                  << "Program will be terminated.\n"
                  << std::endl;
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
