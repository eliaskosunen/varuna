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
#include <array>
#include <stdexcept>

#include "spdlog/fmt/fmt.h"

namespace util
{
	namespace programinfo
	{
		enum VersionStatus : uint8_t
		{
			DEV = 0, RC, STABLE
		};

		struct Version
		{
			// 0 = major, 1 = minor, 2 = patch
			// 3 = rc, 4 = status
			const std::array<uint8_t, 5> data;

			constexpr Version(const std::array<uint8_t, 5> &arr) : data(arr) {}
			constexpr Version(VersionStatus status, uint8_t major, uint8_t minor, uint8_t patch, uint8_t rc = 1) : data({{ major, minor, patch, rc, static_cast<uint8_t>(status) }}) {}

			constexpr VersionStatus getStatus() const
			{
				if(data[4] > 2) throw std::logic_error("Invalid version");
				return static_cast<VersionStatus>(data[4]);
			}

			std::string toString() const
			{
				switch(getStatus())
				{
				case DEV:
					return fmt::format("{}.{}.{}-dev", data[0], data[1], data[2]);
				case RC:
					return fmt::format("{}.{}.{}-rc{}", data[0], data[1], data[2], data[3]);
				case STABLE:
					return fmt::format("{}.{}.{}", data[0], data[1], data[2]);
				}
			}
		};

		constexpr Version version = {DEV, 0, 1, 0};

		constexpr const char *name = "Varuna";
	}
}
