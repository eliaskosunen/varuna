# Varuna

Master branch: [![Build Status](https://travis-ci.org/eliaskosunen/varuna.svg?branch=master)](https://travis-ci.org/eliaskosunen/varuna) [![Build status](https://ci.appveyor.com/api/projects/status/3rjuwbp2ehnuj4lh/branch/master?svg=true)](https://ci.appveyor.com/project/eliaskosunen/varuna/branch/master)

Develop branch: [![Build Status](https://travis-ci.org/eliaskosunen/varuna.svg?branch=develop)](https://travis-ci.org/eliaskosunen/varuna) [![Build status](https://ci.appveyor.com/api/projects/status/3rjuwbp2ehnuj4lh/branch/develop?svg=true)](https://ci.appveyor.com/project/eliaskosunen/varuna/branch/develop)

## About

Varuna is general-purpose interpreter programming language.
The implementation of the language can be found in this repository.
WORK IN PROGRESS.

## Building

### Requirements

The project is developed on Ubuntu 16.04 and tested on Ubuntu 14.04.
Other Linux distributions should work, as long as they'll have the right compiler versions available.
Windows is supported, and macOS support is coming at some point.

A C++11 compliant compiler is required for building.
The project is confirmed to build on
Linux with g++ (versions 4.9.3 and 5.4.0) and Clang (versions 3.6 and 3.8) and
on Windows with Visual Studio 14 (2015). Further support is not planned.

### Instructions

The application can be build using CMake.
The easiest way to do this is to run `build.sh`,
which will build the application, tests and documentation.
`build.sh` will only work on Linux.

If you'd like to build on other platforms than Linux,
or not to build documentation, here are the instructions

### Linux
```sh
# On the repository root folder
$ mkdir build
$ cd build
$ cmake -G "Unix Makefiles" ..
$ make

# Run tests:
$ cd ..
$ ./bin/tests

# Build documentation
$ ./build-docs.sh
```

### Windows

You can either use the CMake GUI or the Command Prompt:
```sh
# On the repository root folder
> md build
> cd build
> cmake -G "Visual Studio 14 2015 Win64" -DCMAKE_BUILD_TYPE=Release ..
```

After that the Visual Studio project files can be found in the folder `build`.

### Requirements

A C++14 compliant compiler is recommended, though a C++11 compiler will work as well.
The application is confirmed to build on Ubuntu 16.04 LTS using g++ 5.4.0, g++ 4.9.3 and clang 3.8.

## License

Varuna is published under the GNU GPL.
The GNU GPL is included in the file LICENSE.

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
