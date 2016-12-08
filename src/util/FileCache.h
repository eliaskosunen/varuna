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

#include "util/StreamReader.h"

#include <unordered_map>
#include <memory>

namespace util
{
	class FileCache
	{
	public:
		class File
		{
		public:
			typedef uint64_t Checksum_t;

			std::string filename, content;
			Checksum_t checksum;

			File(const std::string &filename_)
				: filename(filename_), checksum(0) {}

			bool readFile()
			{
				if(!content.empty())
				{
					return true;
				}
				const util::StreamReader sr;
				content = sr.readFile(filename);
				if(content.empty() || content == "ERROR")
				{
					return false;
				}
				return true;
			}

			void calculateChecksum()
			{
				// TODO
			}
		};

	private:
		std::unordered_map<std::string, std::unique_ptr<File>> cache;

	public:
		FileCache() {}

		File *getFile(const std::string &name)
		{
			return cache.at(name).get();
		}

		bool addFile(std::unique_ptr<File> file, bool read = true)
		{
			if(read)
			{
				if(!file->readFile())
				{
					return false;
				}
			}
			cache.insert(std::make_pair(file->filename, std::move(file)));
			return true;
		}
		bool addFile(const std::string &name, bool read = true)
		{
			auto file = std::make_unique<File>(name);
			return addFile(std::move(file), read);
		}
	};
}
