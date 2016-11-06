/*
Copyright (C) 2016 Elias Kosunen

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include <string>
#include <memory>

#include "tclap.h"

namespace app
{
	/**
	 * Class for handling command line arguments properly
	 */
	class CommandLine
	{
		/**
		 * Count of command line arguments, from main
		 */
		const int argc;
		/**
		 * Command line argument vector, from main
		 */
		char **argv;

		/**
		 * Command line parser
		 */
		TCLAP::CmdLine cmd;

		/**
		 * Show copyright and license information
		 */
		void license() const;

	public:
		/**
		 * CommandLine constructor
		 * @param argc Command line argument count
		 * @param argv Command line argument vector
		 */
		CommandLine(int argc, char **argv);

		/**
		 * Run application.
		 * Processes the command line arguments and
		 * reacts to them properly
		 */
		int run();
	};
}
