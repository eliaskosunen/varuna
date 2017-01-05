// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#pragma once

#include <iterator>

namespace util
{
template <class ReverseIterator>
inline typename ReverseIterator::iterator_type ritToFwdIt(ReverseIterator rit)
{
    return --(rit.base()); // move result of .base() back by one.
}
}
