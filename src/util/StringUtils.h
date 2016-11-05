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
#include <vector>

namespace util
{
	/**
	 * Static string utility functions
	 */
	class StringUtils
	{
	public:
		/**
		 * Replace all occurences of needle with replacement in haystack
		 * @param haystack    String to search in
		 * @param needle      String to replace
		 * @param replacement Replacement string
		 */
		static void replaceAll(std::string &haystack, const std::string &needle, const std::string &replacement)
		{
			size_t start_pos = 0;
			while((start_pos = haystack.find(needle, start_pos)) != std::string::npos) {
				haystack.replace(start_pos, needle.length(), replacement);
				start_pos += replacement.length(); // Handles case where 'replacement' is a substring of 'needle'
			}
		}

		/**
		 * Replace all occurences of needle with replacement in haystack and return a copy
		 * @param  haystack    String to search in
		 * @param  needle      String to replace
		 * @param  replacement Replacement string
		 * @return             Copy of the final string
		 */
		static std::string replaceAllCopy(std::string haystack, const std::string &needle, const std::string &replacement)
		{
			replaceAll(haystack, needle, replacement);
			return haystack;
		}

		/**
		 * Trim off whitespace from the beginning of the string
		 * @param str String to trim
		 */
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

		/**
		 * Trim off whitespace from the end of the string
		 * @param str String to trim
		 */
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

		/**
		 * Trim off whitespace from both sides of the string
		 * @param str String to trim
		 */
		static void trim(std::string &str)
		{
			ltrim(str);
			rtrim(str);
		}

		/**
		 * Trim off whitespace from the beginning of the string and return a copy
		 * @param  str String to trim
		 * @return     Trimmed string
		 */
		static std::string ltrimCopy(std::string str)
		{
			ltrim(str);
			return str;
		}

		/**
		 * Trim off whitespace from the end of the string and return a copy
		 * @param  str String to trim
		 * @return     Trimmed string
		 */
		static std::string rtrimCopy(std::string str)
		{
			rtrim(str);
			return str;
		}

		/**
		 * Trim off whitespace from both sides of the string and return a copy
		 * @param  str String to trim
		 * @return     Trimmed string
		 */
		static std::string trimCopy(std::string str)
		{
			trim(str);
			return str;
		}

		/**
		 * Trim off all consecutive spaces from a string
		 * @param str String to trim
		 */
		static void trimConsecutiveSpaces(std::string &str)
		{
			std::string::iterator newEnd = std::unique(str.begin(), str.end(), [](char lhs, char rhs) { return (lhs == rhs) && (lhs == ' '); });
			str.erase(newEnd, str.end());
		}

		/**
		 * Trim off all consecutive spaces from a string and return a copy
		 * @param  str String to trim
		 * @return     Trimmed string
		 */
		static std::string trimConsecutiveSpacesCopy(std::string str)
		{
			trimConsecutiveSpaces(str);
			return str;
		}

		/**
		 * Convert a C string (const char*) to a C++ string (std::string)
		 * @param  cstr C string to convert
		 * @return      Equivalent C++ string
		 */
		static std::string cstrToString(const char *cstr)
		{
			std::stringstream ss;
			ss << cstr;
			return ss.str();
		}

		/**
		 * Convert a C string (const char*) to a C++ string (std::string) with length information
		 * @param  cstr C string to convert
		 * @param  len  Length of the C string
		 * @return      Equivalent C++ string
		 */
		static std::string cstrToStringLen(const char *cstr, size_t len)
		{
			return std::string(cstr, len);
		}

		/**
		 * Convert a char to C++ string
		 * @param  c Char to convert
		 * @return   Equivalent C++ string
		 */
		static std::string charToString(const char &c)
		{
			std::stringstream ss;
			ss << c;
			return ss.str();
		}

		/**
		 * Is char whitespace
		 * @param  c Char to check
		 * @return   true = whitespace
		 */
		static bool isCharWhitespace(const char &c)
		{
			return std::isspace(static_cast<unsigned char>(c)) != 0;
		}

		/**
		 * Split a string to a vector
		 * @param s     String to split
		 * @param delim Delimeter
		 * @param elems Element vector
		 * @author Evan Teran: http://stackoverflow.com/questions/236129/split-a-string-in-c
		 */
		static void split(const std::string &s, char delim, std::vector<std::string> &elems) {
			std::stringstream ss;
			ss.str(s);
			std::string item;
			while (std::getline(ss, item, delim)) {
				elems.push_back(item);
			}
		}

		/**
		 * Split a string to a vector
		 * @param  s     String to split
		 * @param  delim Delimeter
		 * @return       Element vector
		 * @author Evan Teran: http://stackoverflow.com/questions/236129/split-a-string-in-c
		 */
		static std::vector<std::string> split(const std::string &s, char delim) {
			std::vector<std::string> elems;
			split(s, delim, elems);
			return elems;
		}
	};
}
