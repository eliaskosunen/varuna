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

#include <algorithm>
#include <functional>
#include <cctype>
#include <locale>
#include <string>
#include <sstream>

namespace util
{
	class StringUtils
	{
	public:
		static void replaceAll(std::string &str, const std::string &from, const std::string &to)
		{
			size_t start_pos = 0;
			while((start_pos = str.find(from, start_pos)) != std::string::npos) {
				str.replace(start_pos, from.length(), to);
				start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
			}
		}

		static std::string replaceAllCopy(std::string str, const std::string &from, const std::string &to)
		{
			replaceAll(str, from, to);
			return str;
		}

		static void ltrim(std::string &str)
		{
			str.erase(str.begin(),
				std::find_if(
					str.begin(), str.end(),
					std::not1(
						std::ptr_fun<int, int>(std::isspace)
					)
				)
			);
		}

		static void rtrim(std::string &str)
		{
			str.erase(
				std::find_if(
					str.rbegin(), str.rend(),
					std::not1(
						std::ptr_fun<int, int>(std::isspace)
					)
				).base(), str.end()
			);
		}

		static void trim(std::string &str)
		{
			ltrim(str);
			rtrim(str);
		}

		static std::string ltrimCopy(std::string str)
		{
			ltrim(str);
			return str;
		}

		static std::string rtrimCopy(std::string str)
		{
			rtrim(str);
			return str;
		}

		static std::string trimCopy(std::string str)
		{
			trim(str);
			return str;
		}

		static void trimConsecutiveSpaces(std::string &str)
		{
			std::string::iterator newEnd = std::unique(str.begin(), str.end(), [](char lhs, char rhs) { return (lhs == rhs) && (lhs == ' '); });
			str.erase(newEnd, str.end());
		}

		static std::string trimConsecutiveSpacesCopy(std::string str)
		{
			trimConsecutiveSpaces(str);
			return str;
		}

		static std::string cstrToString(const char *cstr)
		{
			std::stringstream ss;
			ss << cstr;
			return ss.str();
		}

		static std::string charToString(const char &c)
		{
			std::stringstream ss;
			ss << c;
			return ss.str();
		}
	};
}
