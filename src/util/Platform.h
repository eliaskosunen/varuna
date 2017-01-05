/*
Copyright (C) 2016-2017 Elias Kosunen

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

#define CPP_STD_VERSION	__cplusplus
#define CPP_STD_98		199711L
#define CPP_STD_11		201103L
#define CPP_STD_14		201402L

#ifdef _MSC_VER

	#if _MSC_VER >= 1900
		#define CPP_STD_IS_11 0
		#define CPP_STD_IS_14 1
	#else
		#define CPP_STD_IS_11 1
		#define CPP_STD_IS_14 0
	#endif

#else

	#define CPP_STD_IS11	(CPP_STD_VERSION <  CPP_STD_14)
	#define CPP_STD_IS14	(CPP_STD_VERSION >= CPP_STD_14)

	#if !CPP_STD_IS11 && !CPP_STD_14
		#error Error determining C++ Standard version
	#endif

#endif // _MSC_VER

#if CPP_STD_IS11
	#error C++ Standard version not supported
#endif

#define VALUE_TO_STRING(x) #x
#define VALUE(x) VALUE_TO_STRING(x)

#if (defined(DEBUG) || defined(_DEBUG)) && !defined(NDEBUG) && !defined(_NDEBUG)
	#define VARUNA_DEBUG 1
#else
	#define VARUNA_RELEASE 1
#endif
