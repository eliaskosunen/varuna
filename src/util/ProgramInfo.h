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

#include <array>
#include <stdexcept>
#include <string>

#include "spdlog/fmt/fmt.h"

namespace util
{
	namespace programinfo
	{
		namespace version
		{
			enum Status : uint8_t
			{
				STABLE	= 0,
				RC		= 1,
				DEV		= 2
			};

			constexpr uint8_t major		= 0;
			constexpr uint8_t minor		= 1;
			constexpr uint8_t patch		= 0;
			constexpr uint8_t status	= static_cast<uint8_t>(DEV);
			constexpr uint8_t rc		= 0;

			static constexpr Status getStatus()
			{
				static_assert(status <= 2, "Invalid version status");
				return static_cast<Status>(status);
			}

			inline std::string toString()
			{
				switch(getStatus())
				{
				case DEV:
					return fmt::format("{}.{}.{}-dev", major, minor, patch);
				case RC:
					return fmt::format("{}.{}.{}-rc{}", major, minor, patch, rc);
				case STABLE:
					return fmt::format("{}.{}.{}", major, minor, patch);
				}
			}
		}

		constexpr const char *name = "Varuna";
	} // namespace programinfo
} // namespace util
