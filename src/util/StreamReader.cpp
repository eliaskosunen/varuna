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

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <cerrno>
#include <stdexcept>
#include <system_error>
#include <string.h>

#include "util/StreamReader.h"
#include "util/Logger.h"
#include "util/StringUtils.h"

#if defined(_LIBCPP_VERSION) && (_LIBCPP_VERSION >= 1000)
#define HAS_IOS_BASE_FAILURE_DERIVED_FROM_SYSTEM_ERROR 1
#else
#define HAS_IOS_BASE_FAILURE_DERIVED_FROM_SYSTEM_ERROR 0
#endif

namespace util
{
	std::string StreamReader::readStream(std::istream &stream) const
	{
		std::stringstream ss;
		ss << stream.rdbuf();
		return ss.str();
	}

	std::string StreamReader::readFile(const std::string &filename) const
	{
		try {
			std::ifstream s;
			s.exceptions(std::ios::badbit);
			s.open(filename.c_str(), std::ios::binary | std::ios::in);

			if(!s.is_open() || s.fail())
			{
				util::logger->warn("File not open gracefully");
				return "ERROR";
			}

			std::stringstream ss;
			ss << s.rdbuf();
			s.close();
			return ss.str();
		}
		#if HAS_IOS_BASE_FAILURE_DERIVED_FROM_SYSTEM_ERROR

		catch(const std::ios_base::failure &e)
		{
			//
			// e.code() is only available if the lib actually follows iso §27.5.3.1.1
			// and derives ios_base::failure from system_error
			// like e.g. libc++
			// http://llvm.org/viewvc/llvm-project/libcxx/trunk/include/ios?revision=193085&view=markup
			// (line 415)
			//
			// and not keeps on deriving it directly from runtime_error
			// like libstdc++
			// https://github.com/mirrors/gcc/blob/master/libstdc%2B%2B-v3/include/bits/ios_base.h#L209
			//
		catch(const std::ios_base::failure &e)
		#else
		catch(const std::ios_base::failure&)
		#endif
		{
		#if HAS_IOS_BASE_FAILURE_DERIVED_FROM_SYSTEM_ERROR
			util::logger->error("Error reading file: libc++ error #{}: {}", e.code().value(), e.code().message());
		#else
			std::string errmsg;
		#ifdef _MSC_VER
			char buf[80];
			strerror_s<80>(buf, errno);
			errmsg = util::StringUtils::cstrToString(buf);
		#else
			errmsg = strerror(errno);
		#endif // defined _MSC_VER

			util::logger->error("Error reading file: libc error #{}: {}", errno, errmsg);
		}

		#endif // HAS_IOS_BASE_FAILURE_DERIVED_FROM_SYSTEM_ERROR

		return "ERROR";
	}
}
