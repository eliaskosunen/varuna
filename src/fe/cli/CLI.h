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

#include "tclap.h"

namespace fe
{
	namespace cli
	{
		class CLI
		{
			const int argc;
			char **argv;
			TCLAP::CmdLine cmd;

			void showLicense() const;

		public:
			CLI(int argc_, char **argv_)
				: argc(argc_), argv(argv_), cmd("Varuna", ' ', "Pre-Alpha") {}

			int run();
		};
	}
}
