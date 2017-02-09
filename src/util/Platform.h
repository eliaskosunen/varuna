// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#pragma once

#define VALUE_TO_STRING(x) #x
#define VALUE(x) VALUE_TO_STRING(x)

#if(defined(DEBUG) || defined(_DEBUG)) && !defined(NDEBUG) && !defined(_NDEBUG)
#define VARUNA_DEBUG 1
#else
#define VARUNA_RELEASE 1
#endif
