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
#include "util/Logger.h"

#include <string>
#include <unordered_map>
#include <stdexcept>

namespace util
{
	class FileCache
	{
	public:
		class File
		{
		public:
			using Checksum_t = uint64_t;

			std::string filename, content;
			Checksum_t checksum;

			File(std::string filename_)
				: filename(std::move(filename_)), checksum(0) {}

			bool readFile()
			{
				if(!content.empty())
				{
					return true;
				}
				try
				{
					content = StreamReader::readFile(filename);
				}
				catch(const std::exception &e)
				{
					util::logger->error(e.what());
					return false;
				}
				return true;
			}

			[[noreturn]]
			void calculateChecksum()
			{
				throw std::logic_error("util::FileCache::File::calculateChecksum() not implemented");
			}
		};

	private:
		std::unordered_map<std::string, std::unique_ptr<File>> cache;

	public:
		FileCache() {}

		File *getFile(const std::string &name)
		{
			auto fileit = cache.find(name);
			if(fileit == cache.end())
			{
				return nullptr;
			}
			return fileit->second.get();
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
} // namespace util
