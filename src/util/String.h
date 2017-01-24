// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#pragma once

#include <climits>
#include <sstream>
#include <string>

namespace util
{
using char_t = char;
using string_t = std::basic_string<char_t>;
using stringstream_t = std::basic_stringstream<char_t>;

#define CHAR_SIGNED CHAR_MIN < 0
} // namespace util
