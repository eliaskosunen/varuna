# Varuna

**Master branch:**  
Linux: [![Travis Build Status](https://img.shields.io/travis/varuna-lang/varuna/master.svg)](https://travis-ci.org/varuna-lang/varuna)  
Windows: [![Appveyor Build status](https://img.shields.io/appveyor/ci/varuna-lang/varuna/master.svg)](https://ci.appveyor.com/project/varuna-lang/varuna/branch/master)  
Coverage: [![Coverage Status](https://img.shields.io/coveralls/varuna-lang/varuna/master.svg)](https://coveralls.io/github/varuna-lang/varuna?branch=master)

**Develop branch:**  
Linux: [![Travis Build Status](https://img.shields.io/travis/varuna-lang/varuna/develop.svg)](https://travis-ci.org/varuna-lang/varuna)  
Windows: [![Appveyor Build status](https://img.shields.io/appveyor/ci/varuna-lang/varuna/develop.svg)](https://ci.appveyor.com/project/varuna-lang/varuna/branch/develop)  
Coverage: [![Coverage Status](https://img.shields.io/coveralls/varuna-lang/varuna/develop.svg)](https://coveralls.io/github/varuna-lang/varuna?branch=develop)

## About

Varuna is a general-purpose compiled programming language.
The implementation of the language can be found in this repository.
More specifically, this compiler is an LLVM front-end.
It produces (optionally) optimized LLVM IR.

WORK IN PROGRESS. Master branch contains stable, but very outdated and unfunctional code.
Wait for 0.1 release (coming soon(tm)) or grab the code from the develop branch.

## Language tutorial

WORK IN PROGRESS.

A quick guide can be found from quick-guide.md

## Usage

The compiler compiles a Varuna source file (or *module*) to
a native object file `.o` (or optionally some other format) and
a Varuna module file `.vamod` (used for importing).

### Command line usage

```
OVERVIEW: Varuna Compiler
USAGE: varuna [subcommand] [options] Input file list

OPTIONS:

Code generation options:

  Optimization level
    -O0                  - No optimizations (default)
    -O1                  - Enable trivial optimizations
    -O2                  - Enable more optimizations
    -O3                  - Enable expensive optimizations
    -Os                  - Enable size optimizations
    -Oz                  - Enable maximum size optimizations
  -emit                  - Output type
    =none                -   Emit nothing
    =ast                 -   Abstract Syntax Tree
    =llvm-ir             -   LLVM Intermediate Representation '.ll'
    =llvm-bc             -   LLVM Bytecode '.bc'
    =asm                 -   Native assembly '.s'
    =obj                 -   Native object format '.o' (default)
  -g                     - Emit debugging symbols
  -strip-debug           - Strip debug info
  -strip-source-filename - Strip source filename
  -x86-asm-syntax        - Emitted x86 assembly syntax
    =att                 -   AT&T assembly syntax
    =intel               -   Intel assembly syntax

General compiler options:

  -j=<threads>           - Number of worker threads to use (Default: 1)
  -license               - Print license and copyright information
  -logging               - Logging level
    =trace               -   Internal trace messages
    =debug               -   Internal debugging messages
    =info                -   Default logging level
    =warning             -   Log only warnings or greater
    =error               -   Log only errors or greater
    =critical            -   Log only critical messages
    =off                 -   Disable all log messages
  -no-module             - Don't generate module file
  -o=<string>            - Output file

Generic Options:

  -help                  - Display available options (-help-hidden for more)
  -help-list             - Display list of available options (-help-list-hidden for more)
  -version               - Display the version of this program

LLVM options:

  -llvm-as=<string>      - LLVM 'llvm-as' binary
  -llvm-dir=<string>     - LLVM binary directory
  -llvm-llc=<string>     - LLVM 'llc' binary
  -llvm-opt=<string>     - LLVM 'opt' binary
```

## Building

### Requirements

The project is developed on Ubuntu 16.04 and tested on Ubuntu 14.04.
Other Linux distributions should work, as long as they'll have the right compiler versions available.
Windows is supported, and macOS may work, but is not tested due to lack of resources.

A C++14 compliant compiler is required for building.
The project is confirmed to build on
Linux with g++ (versions 5.4.0 and 6.2.0) and Clang (versions 3.6.2 and 3.9.1) and
on Windows with Visual Studio 14 (2015). Other compilers may work, but are not actively supported.
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
# Your LLVM installation directory may be different
# Add -DBUILD_TESTS=OFF to disable building of unit tests
$ cmake -G "Unix Makefiles" -DLLVM_DIR=/usr/lib/llvm-3.9 -DCMAKE_BUILD_TYPE=Release ..
# Parallel builds are recommended
$ make -j4

# Run tests:
$ cd ..
$ ./bin/tests

# Build documentation
./scripts/doxygen.sh
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

Compiler documentation is autogenerated by Doxygen. Its build can be triggered with the script `scripts/doxygen.sh`
After that the docs can be found in HTML and LaTEX formats in the folder `doc`.

WORK IN PROGRESS.
The documentation is lacking in various sections.

## License

Varuna is published under the 3-Clause BSD License ("New/Modified BSD License"),
which is included in the file LICENSE.

Copyright (C) 2016-2017 Elias Kosunen

Varuna also uses various third-party software, see LICENSE.thirdparty.txt for details.
