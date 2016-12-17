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

#include <ostream>
#include <type_traits>

#include "util/Logger.h"

namespace util
{
	template <class Enum_t, class Underlying_t = typename std::underlying_type<Enum_t>::type>
	class SafeEnum
	{
	protected:
		Underlying_t flags;
	public:
		using EnumType = 		Enum_t;
		using UnderlyingType = 	Underlying_t;

		SafeEnum() : flags(static_cast<Enum_t>(0)) {}
		SafeEnum(Enum_t flag) : flags(flag) {}
		SafeEnum(const SafeEnum &orig) = default;

		Underlying_t get() const
		{
			return flags;
		}
		Underlying_t &getRef()
		{
			return flags;
		}

		SafeEnum &operator |=(Enum_t add)
		{
			flags |= static_cast<Underlying_t>(add);
			return *this;
		}
		SafeEnum operator |(Enum_t add)
		{
			SafeEnum result(*this);
			result |= static_cast<Underlying_t>(add);
			return result;
		}
		SafeEnum &operator &=(Enum_t mask)
		{
			flags &= static_cast<Underlying_t>(mask);
			return *this;
		}
		SafeEnum operator &(Enum_t mask)
		{
			SafeEnum result(*this);
			result &= static_cast<Underlying_t>(mask);
			return result;
		}
		SafeEnum operator ~()
		{
			SafeEnum result(*this);
			result.flags = ~result.flags;
			return result;
		}
		SafeEnum &operator ^=(Enum_t mask)
		{
			flags ^= static_cast<Underlying_t>(mask);
			return *this;
		}
		SafeEnum operator ^(Enum_t mask)
		{
			SafeEnum result(*this);
			result ^= static_cast<Underlying_t>(mask);
			return result;
		}

		explicit operator bool() const
		{
			return flags != 0;
		}

		bool operator ==(const Enum_t &b) const
		{
			return flags == b;
		}
		bool operator !=(const Enum_t &b) const
		{
			return !(*this == b);
		}

		bool operator ==(const SafeEnum<Enum_t, Underlying_t> &b) const
		{
			return flags == b.flags;
		}
		bool operator !=(const SafeEnum<Enum_t, Underlying_t> &b) const
		{
			return !(*this == b);
		}

		friend std::ostream &operator <<(std::ostream &o, const Enum_t &t)
		{
			o << static_cast<int>(t);
			return o;
		}

		friend std::ostream &operator <<(std::ostream &o, const util::SafeEnum<Enum_t, Underlying_t> &t)
		{
			o << t.get();
			return o;
		}

		bool isSet(Enum_t flag) const
		{
			return (static_cast<Underlying_t>(flags) & static_cast<Underlying_t>(flag)) != static_cast<Underlying_t>(0);
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
