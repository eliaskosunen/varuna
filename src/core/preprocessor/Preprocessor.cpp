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

#include "core/preprocessor/Preprocessor.h"

#include <string>

#include "util/StringUtils.h"

namespace core
{
	namespace preprocessor
	{
		std::string Preprocessor::run(const std::string &str) const
		{
			std::string processed = str;;

			util::StringUtils::replaceAll(processed, "\t", " ");
			util::StringUtils::replaceAll(processed, "\n", " ");
			util::StringUtils::replaceAll(processed, "\r", "");
			util::StringUtils::trim(processed);
			util::StringUtils::trimConsecutiveSpaces(processed);

			return processed;
		}
	}
}
