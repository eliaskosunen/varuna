// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#pragma once

// C++ standard version
#define CPP_STD_VERSION __cplusplus
#define CPP_STD_98 199711L
#define CPP_STD_11 201103L
#define CPP_STD_14 201402L

#ifdef _MSC_VER

// __cplusplus doesn't work on MSVC
#if _MSC_VER >= 1900
#define CPP_STD_IS_11 0
#define CPP_STD_IS_14 1
#else
#define CPP_STD_IS_11 1
#define CPP_STD_IS_14 0
#endif

#else

#define CPP_STD_IS11 (CPP_STD_VERSION < CPP_STD_14)
#define CPP_STD_IS14 (CPP_STD_VERSION >= CPP_STD_14)

#if !CPP_STD_IS11 && !CPP_STD_14
#error Error determining C++ Standard version
#endif

#endif // _MSC_VER

// C++14 or newer is required
#if CPP_STD_IS11
#error C++ Standard version not supported
#endif

#define VALUE_TO_STRING(x) #x
#define VALUE(x) VALUE_TO_STRING(x)

#if(defined(DEBUG) || defined(_DEBUG)) && !defined(NDEBUG) && !defined(_NDEBUG)
#define VARUNA_DEBUG 1
#else
#define VARUNA_RELEASE 1
#endif
