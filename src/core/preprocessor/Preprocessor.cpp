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
#include <algorithm>
#include <iostream>
#include <cstring>

#include "util/StringUtils.h"

namespace core
{
	namespace preprocessor
	{
		std::string Preprocessor::run(const std::string &str) const
		{
			std::string processed;
			std::vector<std::string> vec = util::StringUtils::split(str, '\n');

			std::cout << "str.empty(): " << str.empty() << "\n";
			std::cout << "vec.empty(): " << vec.empty() << "\n";

			for(auto it = vec.begin(); it != vec.end(); ++it)
			{
				std::string row = *it;
				const char *rowCstr = row.c_str();
				std::cout << "\nrow: " << row << "\n";
				const char *commentSymbolPos = std::strchr(rowCstr, '#');
				if(commentSymbolPos != 0)
				{
					int pos = commentSymbolPos - rowCstr;
					std::cout << "pos: " << pos << "\n";
					row.erase(row.begin() + pos, row.end());
				}

				util::StringUtils::replaceAll(row, "\t", " ");
				util::StringUtils::replaceAll(row, "\n", " ");
				util::StringUtils::replaceAll(row, "\r", "");
				util::StringUtils::trim(row);
				util::StringUtils::trimConsecutiveSpaces(row);

				std::cout << "\nprocessed row: " << row << "\n";
				processed.append(row);
			}

			return processed;
		}
	}
}
