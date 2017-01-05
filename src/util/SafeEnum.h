// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#pragma once

#include "util/Logger.h"
#include <ostream>
#include <type_traits>

namespace util
{
template <typename Enum_t,
          typename Underlying_t = typename std::underlying_type<Enum_t>::type>
class SafeEnum
{
protected:
    Underlying_t flags{static_cast<Underlying_t>(0)};

public:
    using EnumType = Enum_t;
    using UnderlyingType = Underlying_t;

    SafeEnum() = default;
    SafeEnum(Enum_t flag) : flags(flag)
    {
    }

    SafeEnum(const SafeEnum&) = default;
    SafeEnum(SafeEnum&&) = default;

    SafeEnum& operator=(const SafeEnum&) = default;
    SafeEnum& operator=(SafeEnum&&) = default;

    ~SafeEnum() = default;

    Underlying_t get() const
    {
        return flags;
    }
    Underlying_t& getRef()
    {
        return flags;
    }

    SafeEnum& operator|=(Enum_t add)
    {
        flags |= static_cast<Underlying_t>(add);
        return *this;
    }
    SafeEnum operator|(Enum_t add)
    {
        SafeEnum result(*this);
        result |= static_cast<Underlying_t>(add);
        return result;
    }
    SafeEnum& operator&=(Enum_t mask)
    {
        flags &= static_cast<Underlying_t>(mask);
        return *this;
    }
    SafeEnum operator&(Enum_t mask)
    {
        SafeEnum result(*this);
        result &= static_cast<Underlying_t>(mask);
        return result;
    }
    SafeEnum operator~()
    {
        SafeEnum result(*this);
        result.flags = ~result.flags;
        return result;
    }
    SafeEnum& operator^=(Enum_t mask)
    {
        flags ^= static_cast<Underlying_t>(mask);
        return *this;
    }
    SafeEnum operator^(Enum_t mask)
    {
        SafeEnum result(*this);
        result ^= static_cast<Underlying_t>(mask);
        return result;
    }

    explicit operator bool() const
    {
        return flags != 0;
    }

    bool operator==(const Enum_t& b) const
    {
        return flags == static_cast<Underlying_t>(b);
    }
    bool operator!=(const Enum_t& b) const
    {
        return !(*this == b);
    }

    bool operator==(const SafeEnum<Enum_t, Underlying_t>& b) const
    {
        return flags == b.flags;
    }
    bool operator!=(const SafeEnum<Enum_t, Underlying_t>& b) const
    {
        return !(*this == b);
    }

    friend std::ostream& operator<<(std::ostream& o, const Enum_t& t)
    {
        o << static_cast<int>(t);
        return o;
    }

    friend std::ostream&
    operator<<(std::ostream& o, const util::SafeEnum<Enum_t, Underlying_t>& t)
    {
        o << t.get();
        return o;
    }

    bool isSet(Enum_t flag) const
    {
        return (static_cast<Underlying_t>(flags) &
                static_cast<Underlying_t>(flag)) !=
               static_cast<Underlying_t>(0);
    }
    bool isNotSet(Enum_t flag) const
    {
        return !isSet(flag);
    }
    void set(Enum_t flag)
    {
        flags |= flag;
    }
    void reset()
    {
        flags = static_cast<Enum_t>(0);
    }
};
} // namespace util
