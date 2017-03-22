// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#pragma once

#ifdef _MSC_VER
#pragma warning(push, 0)
#endif

#include <guid/guid.h>
#include <sstream>

inline std::string guidToString(const Guid& i)
{
    std::stringstream ss;
    ss << i;
    return ss.str();
}

#ifdef _MSC_VER
#pragma warning(pop)
#endif
