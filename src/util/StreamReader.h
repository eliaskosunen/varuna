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

#include <istream>
#include <memory>
#include <stdexcept>

namespace util
{
/**
 * Read an istream
 */
class StreamReader
{
public:
    /**
     * Read a file
     * @param  filename Name of the file to read
     * @return          Contents of the file
     */
    static std::string readFile(const std::string& filename);
};
} // namespace util
