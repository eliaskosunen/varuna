# Varuna

Master branch: [![Build Status](https://travis-ci.org/eliaskosunen/varuna.svg?branch=master)](https://travis-ci.org/eliaskosunen/varuna)

Develop branch: [![Build Status](https://travis-ci.org/eliaskosunen/varuna.svg?branch=develop)](https://travis-ci.org/eliaskosunen/varuna)

## About

Varuna is general-purpose interpreter programming language.
The implementation of the language can be found in this repository.
WORK IN PROGRESS.

## Building

The easiest way to build the application is to use GNU Make.
To build, run `$ make build`, which will provide you the executable.
To also build the unit tests and documentation, it's recommended to use `build.sh`.
Binaries can be found in the directory `bin/`.

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
