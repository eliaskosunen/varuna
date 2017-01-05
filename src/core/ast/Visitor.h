/*
Copyright (C) 2016-2017 Elias Kosunen

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

namespace core
{
	namespace ast
	{
		class Visitor
		{
		public:
			Visitor() = default;

			Visitor(const Visitor&) = delete;
			Visitor(Visitor&&) = default;

			Visitor &operator =(const Visitor&) = delete;
			Visitor &operator =(Visitor&&) = default;

			virtual ~Visitor() = default;
		};
	} // namespace ast
} // namespace core
