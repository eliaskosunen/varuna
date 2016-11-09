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

#include <memory>

#define CPP_STD_VERSION	__cplusplus
#define CPP_STD_98		199711L
#define CPP_STD_11		201103L
#define CPP_STD_14		201402L

#define VALUE_TO_STRING(x) #x
#define VALUE(x) VALUE_TO_STRING(x)

namespace util
{
#if CPP_STD_VERSION < CPP_STD_14 // Not C++14
	/**
	 * std::make_unique was added to the standard in C++14.
	 * Implement our own version of it if standard is C++11 or lower
	 */
	template<typename T, typename... Args>
	std::unique_ptr<T> make_unique(Args&&... args)
	{
		return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
	}
#else // C++14 or higher
	template<typename T, typename... Args>
	std::unique_ptr<T> make_unique(Args&&... args)
	{
		return std::make_unique<T>(args...);
	}
#endif
}
