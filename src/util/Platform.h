// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#pragma once

#define VALUE_TO_STRING(x) #x
#define VALUE(x) VALUE_TO_STRING(x)

#ifdef _MSC_VER
#define VARUNA_MSVC 1
#else
#define VARUNA_MSVC 0
#endif

#if VARUNA_MSVC
#if defined(UNICODE) || defined(_UNICODE)
#define VARUNA_MSVC_UNICODE 1
#else
#define VARUNA_MSVC_UNICODE 0
#endif // defined (UNICODE)
#else
#define VARUNA_MSVC_UNICODE 0
#endif // VARUNA_MSVC
