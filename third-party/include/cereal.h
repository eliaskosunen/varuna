// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#pragma once

#ifdef _MSC_VER

#pragma warning(push, 0)

#endif

#define CEREAL_THREAD_SAFE 1

#include "cereal/access.hpp"
#include "cereal/cereal.hpp"
#include "cereal/types/memory.hpp"
#include "cereal/types/unordered_map.hpp"
#include "cereal/types/vector.hpp"

#ifdef _MSC_VER

#pragma warning(pop)

#endif
