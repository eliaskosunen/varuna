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

#pragma once

#include "util/Platform.h"

#include <cstdint>

static_assert(sizeof(int8_t) >= 1, "int8_t has to at least be 8 bits wide");
static_assert(sizeof(int16_t) >= 2, "int16_t has to at least be 16 bits wide");
static_assert(sizeof(int32_t) >= 4, "int32_t has to at least be 32 bits wide");
static_assert(sizeof(int64_t) >= 8, "int64_t has to at least be 64 bits wide");

static_assert(sizeof(uint8_t) >= 1, "uint8_t has to be at least 8 bits wide");
static_assert(sizeof(uint16_t) >= 2, "uint16_t has to be at least 16 bits wide");
static_assert(sizeof(uint32_t) >= 4, "uint32_t has to be at least 32 bits wide");
static_assert(sizeof(uint64_t) >= 8, "uint64_t has to be at least 64 bits wide");
