# Varuna

**Master branch:**  
Linux/OSX: [![Travis Build Status](https://img.shields.io/travis/varuna-lang/varuna/master.svg)](https://travis-ci.org/varuna-lang/varuna)  
Windows: [![Appveyor Build status](https://img.shields.io/appveyor/ci/varuna-lang/varuna/master.svg)](https://ci.appveyor.com/project/varuna-lang/varuna/branch/master)  
Coverage: [![Coverage Status](https://img.shields.io/coveralls/varuna-lang/varuna/master.svg)](https://coveralls.io/github/varuna-lang/varuna?branch=master)

**Develop branch:**  
Linux/OSX: [![Travis Build Status](https://img.shields.io/travis/varuna-lang/varuna/develop.svg)](https://travis-ci.org/varuna-lang/varuna)  
Windows: [![Appveyor Build status](https://img.shields.io/appveyor/ci/varuna-lang/varuna/develop.svg)](https://ci.appveyor.com/project/varuna-lang/varuna/branch/develop)  
Coverage: [![Coverage Status](https://img.shields.io/coveralls/varuna-lang/varuna/develop.svg)](https://coveralls.io/github/varuna-lang/varuna?branch=develop)

## About

Varuna is a general-purpose compiled programming language.
The implementation of the language can be found in this repository.
More specifically, this compiler is an LLVM front-end.
It produces either native object code, assembly or LLVM IR or bytecode.

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
USAGE: varuna [subcommand] [options] Input file

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

### Compiling a Varuna program

Building a Varuna program is currently not very user-friendly as you'll have to link the object files manually using the linker of your operating system.

```sh
# Produces program.o
$ varuna program.va
# Link the object files
# This example uses gcc
$ gcc program.o -lvart -lvacstd -o a.out
# Run it!
$ ./a.out
```

## Building

### Requirements

The project is developed on Ubuntu 16.04 and tested on Ubuntu 14.04.
Other Linux distributions should work, as long as they'll have the right compiler versions available.
Building on Windows is supported and tested on Win10.
Mac OS X may work, but is not tested due to lack of resources.

This project depends on LLVM. It needs the LLVM binaries 'llvm-as', 'llc' and 'opt' available in the system path and the LLVM compiler infrastructure C++ libraries.
The project is developed on version 3.9 and guaranteed to work only on that version.
The command line flag -DLLVM_DIR can be used if CMake cannot find the LLVM installation from your system.

To build the compiler, a C++14 standard compliant compiler is required. This means g++-5 or up (g++-4.9 may work) or clang++-3.6 or up (clang++-3.4 or 3.5 may work).
On Windows, only Visual Studio 14 (2015) is supported.

### Installing LLVM

The easiest way to install LLVM, if you are on Debian/Ubuntu, is to install them with `apt-get`. ([More information on the LLVM webpage](http://apt.llvm.org/)). First, add the apt repository:

```
# Debian (Jessie)
deb http://apt.llvm.org/jessie/ llvm-toolchain-jessie-3.9 main
deb-src http://apt.llvm.org/jessie/ llvm-toolchain-jessie-3.9 main
# Ubuntu (Xenial)
deb http://apt.llvm.org/xenial/ llvm-toolchain-xenial-3.9 main
deb-src http://apt.llvm.org/xenial/ llvm-toolchain-xenial-3.9 main
# See instructions for other OS versions from the link above
```

Then do the rest of the installation in the shell:

```sh
# Retrieve archive signature
$ wget -O - http://apt.llvm.org/llvm-snapshot.gpg.key | sudo apt-key add -

# Install packages
$ sudo apt-get install libllvm3.9 llvm-3.9 llvm-3.9-dev

# LLVM packages will be on /usr/lib/llvm-3.9
```

On other platforms, you need to build LLVM yourself.
See the LLVM documentation:
[Linux](http://llvm.org/docs/GettingStarted.html)
[Windows](http://llvm.org/docs/GettingStartedVS.html)
[Downloads](releases.llvm.org/download.html#3.9.1)

### Linux

```sh
# On the repository root folder
$ mkdir build
$ cd build
# Your LLVM installation directory may be different
$ cmake -G "Unix Makefiles" -DLLVM_DIR=/usr/lib/llvm-3.9 -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS_OFF ..
# Parallel builds are recommended
$ make -j4
$ sudo make install
```

### Windows

You can either use the CMake GUI to create the MSVC project files.

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
