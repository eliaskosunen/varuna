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

#include <vector>
#include <string>

namespace util
{

/**
	@author
		iain: http://stackoverflow.com/questions/865668/how-to-parse-command-line-arguments-in-c
	*/
class CommandLineParser
{
    std::vector<std::string> tokens;

  public:
    CommandLineParser(int &argc, char **argv);
	CommandLineParser(int &argc, const char **argv);

    const std::string &getOption(const std::string &option) const;
    bool optionExists(const std::string &option) const;

	int size() const;
	bool empty() const;
};
}