# Varuna

**Master branch:**  
Linux: [![Travis Build Status](https://img.shields.io/travis/varuna-lang/varuna/master.svg)](https://travis-ci.org/varuna-lang/varuna)  
Windows: [![Appveyor Build status](https://img.shields.io/appveyor/ci/varuna-lang/varuna/master.svg)](https://ci.appveyor.com/project/varuna-lang/varuna/branch/master)

**Develop branch:**  
Linux: [![Travis Build Status](https://img.shields.io/travis/varuna-lang/varuna/develop.svg)](https://travis-ci.org/varuna-lang/varuna)  
Windows: [![Appveyor Build status](https://img.shields.io/appveyor/ci/varuna-lang/varuna/develop.svg)](https://ci.appveyor.com/project/varuna-lang/varuna/branch/develop)

## About

Varuna is a general-purpose compiled programming language.
The implementation of the language can be found in this repository.
More specifically, this compiler is an LLVM front-end.

WORK IN PROGRESS. Master branch contains stable, but very outdated and unfunctional code.
Wait for 0.1 release (coming soon(tm)) or grab the code from the develop branch.

## Building

### Requirements

The project is developed on Ubuntu 16.04 and tested on Ubuntu 14.04.
Other Linux distributions should work, as long as they'll have the right compiler versions available.
Windows is supported, and macOS may work, but is not tested due to lack of resources.

A C++14 compliant compiler is required for building.
The project is confirmed to build on
Linux with g++ (versions 5.4.0 and 6.2.0) and Clang (versions 3.6.2 and 3.8.0) and
on Windows with Visual Studio 14 (2015). Older compilers may work, but are not actively supported.
Further support is not planned.

This project depends on LLVM. It is developed on version 3.9, but earlier versions may work as well.
The command line flag -DLLVM_DIR can be used if CMake cannot find the LLVM installation from your system.

### Instructions

The application can be built using CMake.
The easiest way to do this is to run `build.sh`,
which will build the application, tests and documentation.
`build.sh` will only work on Linux.

If you'd like to build on other platforms than Linux,
or not to build documentation, here are the instructions:

### Linux

```sh
# On the repository root folder
$ mkdir build
$ cd build
$ cmake -G "Unix Makefiles" -DLLVM_DIR=/usr/lib/llvm-3.9/lib/cmake/llvm ..
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

## Contributing

See CONTRIBUTING.md

## Compiler documentation

### Directory structure

```
src/ ---------- Source directory
- core/ --------- Compiler essentials (namespace: core)
--- ast/ ---------- Abstract syntax tree representation (core::ast)
--- codegen/ ------ Code generation (core::codegen)
--- lexer/ -------- Lexical analyzer (core::lexer)
--- parser/ ------- Parser (core::parser)
- fe/ ----------- "Front-end", abstraction layers over core for easier development (fe)
--- api/ ---------- Basic API (fe::api)
--- cli/ ---------- Command line interface that builds on top of api (fe::cli)
- tests/ -------- Unit tests
- util/ --------- Project-wide utilities (util)
third-party/ -- Third-party libraries
```

## License

Varuna is published under the GNU GPL.
The GNU GPL is included in the file LICENSE.

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

Varuna also uses various third-party software, see LICENSE.thirdparty.txt for details.
