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

#include <algorithm>
#include <vector>
#include <string>

#include "util/CommandLineParser.h"

namespace util
{

const std::string CommandLineParser::DEFAULT = "";

CommandLineParser::CommandLineParser(int &argc, char **argv)
{
	for(int i = 1; i < argc; ++i)
	{
		tokens.push_back(std::string(argv[i]));
	}
}

CommandLineParser::CommandLineParser(int &argc, const char **argv)
{
	for(int i = 1; i < argc; ++i)
	{
		tokens.push_back(std::string(argv[i]));
	}
}

const std::string &CommandLineParser::getOption(const std::string &option) const
{
	auto itr = std::find(tokens.begin(), tokens.end(), option);
	if(itr != tokens.end() && ++itr != tokens.end())
	{
		return *itr;
	}
	return DEFAULT;
}

bool CommandLineParser::optionExists(const std::string &option) const
{
	return std::find(tokens.begin(), tokens.end(), option) != tokens.end();
}

int CommandLineParser::size() const
{
	return tokens.size();
}

bool CommandLineParser::empty() const
{
	return tokens.empty();
}

}
