# Varuna

[![License](https://img.shields.io/github/license/eliaskosunen/varuna.svg)](LICENSE)

**Master branch:**  
Linux: [![Travis Build Status](https://img.shields.io/travis/eliaskosunen/varuna/master.svg)](https://travis-ci.org/eliaskosunen/varuna)  
Windows: [![Appveyor Build status](https://img.shields.io/appveyor/ci/eliaskosunen/varuna/master.svg)](https://ci.appveyor.com/project/eliaskosunen/varuna/branch/master)

**Develop branch:**  
Linux: [![Travis Build Status](https://img.shields.io/travis/eliaskosunen/varuna/develop.svg)](https://travis-ci.org/eliaskosunen/varuna)  
Windows: [![Appveyor Build status](https://img.shields.io/appveyor/ci/eliaskosunen/varuna/develop.svg)](https://ci.appveyor.com/project/eliaskosunen/varuna/branch/develop)

## About

Varuna is a general-purpose interpreted programming language.
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

Varuna also uses various third-party software, see LICENSE.thirdparty.txt for details.
