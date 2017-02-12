// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#pragma once

#ifdef _MSC_VER

#pragma warning(push, 3)
#pragma warning(disable : 4514 4711)

#endif

#include "cereal/archives/binary.hpp"
#include "cereal/archives/json.hpp"
#include "cereal/archives/portable_binary.hpp"
#include "cereal/archives/xml.hpp"

#ifdef _MSC_VER

#pragma warning(pop)

#endif
