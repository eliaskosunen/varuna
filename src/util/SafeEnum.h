// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#pragma once

#include "util/Logger.h"
#include <cereal.h>
#include <ostream>
#include <type_traits>

namespace util
{
/// Typesafe enum type
template <typename EnumT,
          typename UnderlyingT = typename std::underlying_type<EnumT>::type>
class SafeEnum
{
public:
    using EnumType = EnumT;
    using UnderlyingType = UnderlyingT;

    SafeEnum() = default;
    /*implicit*/ SafeEnum(EnumT flag) : flags(flag)
    {
    }

    SafeEnum(const SafeEnum&) = default;
    SafeEnum(SafeEnum&&) noexcept(
        std::is_nothrow_move_constructible<UnderlyingT>::value) = default;

    SafeEnum& operator=(const SafeEnum&) = default;
    SafeEnum& operator=(SafeEnum&&) noexcept(
        std::is_nothrow_move_assignable<UnderlyingT>::value) = default;

    ~SafeEnum() noexcept = default;

    /// Get underlying value
    UnderlyingT get() const
    {
        return flags;
    }
    /// Get reference to the underlying value
    UnderlyingT& getRef()
    {
        return flags;
    }

    SafeEnum& operator|=(EnumT add)
    {
        flags |= static_cast<UnderlyingT>(add);
        return *this;
    }
    SafeEnum operator|(EnumT add)
    {
        SafeEnum result(*this);
        result |= static_cast<UnderlyingT>(add);
        return result;
    }
    SafeEnum& operator&=(EnumT mask)
    {
        flags &= static_cast<UnderlyingT>(mask);
        return *this;
    }
    SafeEnum operator&(EnumT mask)
    {
        SafeEnum result(*this);
        result &= static_cast<UnderlyingT>(mask);
        return result;
    }
    SafeEnum operator~()
    {
        SafeEnum result(*this);
        result.flags = ~result.flags;
        return result;
    }
    SafeEnum& operator^=(EnumT mask)
    {
        flags ^= static_cast<UnderlyingT>(mask);
        return *this;
    }
    SafeEnum operator^(EnumT mask)
    {
        SafeEnum result(*this);
        result ^= static_cast<UnderlyingT>(mask);
        return result;
    }

    /// Explicitly convertible to bool
    explicit operator bool() const
    {
        return flags != 0;
    }

    bool operator==(const EnumT& b) const
    {
        return flags == static_cast<UnderlyingT>(b);
    }
    bool operator!=(const EnumT& b) const
    {
        return !(*this == b);
    }

    bool operator==(const SafeEnum<EnumT, UnderlyingT>& b) const
    {
        return flags == b.flags;
    }
    bool operator!=(const SafeEnum<EnumT, UnderlyingT>& b) const
    {
        return !(*this == b);
    }

    friend std::ostream& operator<<(std::ostream& o, const EnumT& t)
    {
        o << static_cast<UnderlyingT>(t);
        return o;
    }

    friend std::ostream& operator<<(std::ostream& o,
                                    const util::SafeEnum<EnumT, UnderlyingT>& t)
    {
        o << t.get();
        return o;
    }

    /// Is a flag set
    bool isSet(EnumT flag) const
    {
        return (static_cast<UnderlyingT>(flags) &
                static_cast<UnderlyingT>(flag)) != static_cast<UnderlyingT>(0);
    }
    /// Is a flag not set
    bool isNotSet(EnumT flag) const
    {
        return !isSet(flag);
    }
    /// Set a flag
    void set(EnumT flag)
    {
        flags |= flag;
    }
    /// Reset all flags
    void reset()
    {
        flags = static_cast<EnumT>(0);
    }

    /**
     * Convert to other type.
     * Unsafe, doesn't check if types are related
     */
    template <typename T>
    T convert() const
    {
        return T{static_cast<typename T::EnumType>(get())};
    }

    template <class Archive>
    void serialize(Archive& archive)
    {
        archive(CEREAL_NVP(flags));
    }

protected:
    /// Underlying flags
    UnderlyingT flags{static_cast<UnderlyingT>(0)};
};
} // namespace util
