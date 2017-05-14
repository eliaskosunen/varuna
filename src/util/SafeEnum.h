// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#pragma once

#include <cereal.h>
#include <ostream>
#include <type_traits>

#if defined(_MSC_VER) && _MSC_VER < 1910
#pragma push_macro("constexpr")
#define constexpr /*constexpr*/
#endif

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

    constexpr SafeEnum() = default;
    constexpr /*implicit*/ SafeEnum(EnumT flag) : flags(flag)
    {
    }

    constexpr SafeEnum(const SafeEnum&) = default;
    constexpr SafeEnum(SafeEnum&&) noexcept(
        std::is_nothrow_move_constructible<UnderlyingT>::value) = default;

    constexpr SafeEnum& operator=(const SafeEnum&) = default;
    constexpr SafeEnum& operator=(SafeEnum&&) noexcept(
        std::is_nothrow_move_assignable<UnderlyingT>::value) = default;

    ~SafeEnum() noexcept = default;

    /// Get underlying value
    constexpr UnderlyingT& get() noexcept
    {
        return flags;
    }
    /// Get underlying value
    constexpr const UnderlyingT& get() const noexcept
    {
        return flags;
    }

    constexpr SafeEnum& operator|=(EnumT add) noexcept
    {
        flags |= static_cast<UnderlyingT>(add);
        return *this;
    }
    constexpr SafeEnum operator|(EnumT add) noexcept
    {
        SafeEnum result(*this);
        result |= static_cast<UnderlyingT>(add);
        return result;
    }
    constexpr SafeEnum& operator&=(EnumT mask) noexcept
    {
        flags &= static_cast<UnderlyingT>(mask);
        return *this;
    }
    constexpr SafeEnum operator&(EnumT mask) noexcept
    {
        SafeEnum result(*this);
        result &= static_cast<UnderlyingT>(mask);
        return result;
    }
    constexpr SafeEnum operator~() noexcept
    {
        SafeEnum result(*this);
        result.flags = ~result.flags;
        return result;
    }
    constexpr SafeEnum& operator^=(EnumT mask) noexcept
    {
        flags ^= static_cast<UnderlyingT>(mask);
        return *this;
    }
    constexpr SafeEnum operator^(EnumT mask) noexcept
    {
        SafeEnum result(*this);
        result ^= static_cast<UnderlyingT>(mask);
        return result;
    }

    /// Explicitly convertible to bool
    constexpr explicit operator bool() const noexcept
    {
        return flags != 0;
    }

    constexpr bool operator==(const EnumT& b) const noexcept
    {
        return flags == static_cast<UnderlyingT>(b);
    }
    constexpr bool operator!=(const EnumT& b) const noexcept
    {
        return !(*this == b);
    }

    constexpr bool operator==(const SafeEnum<EnumT, UnderlyingT>& b) const
        noexcept
    {
        return flags == b.flags;
    }
    constexpr bool operator!=(const SafeEnum<EnumT, UnderlyingT>& b) const
        noexcept
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
    constexpr bool isSet(EnumT flag) const noexcept
    {
        return (static_cast<UnderlyingT>(flags) &
                static_cast<UnderlyingT>(flag)) != static_cast<UnderlyingT>(0);
    }
    /// Is a flag not set
    constexpr bool isNotSet(EnumT flag) const noexcept
    {
        return !isSet(flag);
    }
    /// Set a flag
    constexpr void set(EnumT flag) noexcept
    {
        flags |= flag;
    }
    /// Reset all flags
    constexpr void reset() noexcept
    {
        flags = static_cast<EnumT>(0);
    }

    /**
     * Convert to other type.
     * Unsafe, doesn't check if types are related
     */
    template <typename T>
    constexpr T convert() const noexcept
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

#if defined(_MSC_VER) && _MSC_VER < 1910
#undef constexpr
#pragma pop_macro("constexpr")
#endif
