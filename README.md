# Varuna

```varuna
// Example Varuna program
// Compile with:
// $ varuna example.va -O3 -o example.o
// You'll have to link it yourself

// Declare module
module example;

// Import C standard library
import cstd;

// Define program entry point
def main() -> i32 {
    // Write to stdout
    putstr("Hello world!");
    return 0;
}
```

## Build status

|                | Master branch | Develop branch |
| -------------- | ------------- | -------------- |
| **Linux/OS X** | [![Travis Build Status](https://travis-ci.org/varuna-lang/varuna.svg?branch=master)](https://travis-ci.org/varuna-lang/varuna) | [![Travis Build Status](https://travis-ci.org/varuna-lang/varuna.svg?branch=develop)](https://travis-ci.org/varuna-lang/varuna) |
| **Windows**    | [![Appveyor Build Status](https://ci.appveyor.com/api/projects/status/l52k63sfb6x6ar0e/branch/master?svg=true)](https://ci.appveyor.com/project/varuna-lang/varuna/branch/master)  | [![Appveyor Build Status](https://ci.appveyor.com/api/projects/status/l52k63sfb6x6ar0e/branch/develop?svg=true)](https://ci.appveyor.com/project/varuna-lang/varuna/branch/develop) |
| **Coverage**   | [![Coverage Status](https://coveralls.io/repos/github/varuna-lang/varuna/badge.svg?branch=master)](https://coveralls.io/github/varuna-lang/varuna?branch=master) | [![Coverage Status](https://coveralls.io/repos/github/varuna-lang/varuna/badge.svg?branch=develop)](https://coveralls.io/github/varuna-lang/varuna?branch=develop) |

## About

Varuna is a general-purpose compiled programming language.
The implementation of the language can be found in this repository.
More specifically, this compiler is an LLVM front-end.
It produces either native object code, assembly or LLVM IR or bytecode.

WORK IN PROGRESS. 0.1 release is coming soon.

## Language documentation

WORK IN PROGRESS.

A quick guide can be found from quick-guide.md  
[Reference manual (WIP)](https://varuna-lang.gitbooks.io/varuna-book/content/)  
[API Reference](https://varuna-lang.gitbooks.io/varuna-api-documentation/content/)

## Usage

The compiler compiles a Varuna source file (or *module*) to
a native object file `.o`/`.obj` (or optionally some other format) and
a Varuna module file `.vamod` (used for importing).

### Command line usage

```
OVERVIEW: Varuna Compiler
USAGE: varuna [options] Input file

OPTIONS:

Generic Options:

  -help                  - Display available options (-help-hidden for more)
  -help-list             - Display list of available options (-help-list-hidden for more)
  -version               - Display the version of this program

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
```

### Compiling a Varuna program

Building a Varuna program is currently not very user-friendly as you'll have to link the object files manually using the linker of your operating system.

```sh
# Produces program.o
$ varuna program.va
# Link the object files
# This example uses gcc
$ gcc program.o -lvart -lvastd -o a.out
# Run it!
$ ./a.out
```

## Building

### AUR

There's an AUR for the Varuna compiler, that should be used on Arch Linux:
https://aur.archlinux.org/packages/varuna-git/

### Requirements

The project is developed on Arch Linux and tested on Ubuntu 12.04.
Other Linux distributions should work, as long as they'll have the right compiler and library versions available.
Building on Windows is supported and tested on Win10.
Building on Mac OS X is supported and tested.

This project depends on LLVM and needs the LLVM compiler infrastructure C++ libraries.
The project is developed on version 4.0 and guaranteed to work only on that version.
We are trying to also keep the code compatible with upcoming LLVM releases (5.0).
Every release drops support for previous LLVM versions.
The command line flag -DLLVM_DIR can be used if CMake cannot find the LLVM installation from your system.

To build the compiler, a C++14 standard compliant compiler is required. This means g++-5 or up (g++-4.9 may work) or clang++-3.6 or up.
On Windows, Visual Studio 14 (2015) or up is supported.

### Installing LLVM

#### Debian/Ubuntu

On Debian/Ubuntu LLVM can be installed with `apt-get`. ([More information on the LLVM webpage](http://apt.llvm.org/)). First, add the apt repository:

```
# Debian (Jessie)
deb http://apt.llvm.org/jessie/ llvm-toolchain-jessie-4.0 main
deb-src http://apt.llvm.org/jessie/ llvm-toolchain-jessie-4.0 main
# Ubuntu (Xenial)
deb http://apt.llvm.org/xenial/ llvm-toolchain-xenial-4.0 main
deb-src http://apt.llvm.org/xenial/ llvm-toolchain-xenial-4.0 main
# See instructions for other OS versions from the link above
```

Then do the rest of the installation in the shell:

```sh
# Add sources (see above)
$ echo "deb http://apt.llvm.org/xenial/ llvm-toolchain-xenial-4.0 main" | sudo tee -a /etc/apt/sources.list > /dev/null
$ echo "deb-src http://apt.llvm.org/xenial/ llvm-toolchain-xenial-4.0 main" | sudo tee -a /etc/apt/sources.list > /dev/null

# Retrieve archive signature
$ wget -O - http://apt.llvm.org/llvm-snapshot.gpg.key | sudo apt-key add -

# Update package lists
$ sudo apt-get update

# Install packages
$ sudo apt-get install libllvm4.0 llvm-4.0 llvm-4.0-dev

# LLVM packages will be in /usr/lib/llvm-4.0
```

#### Arch Linux

Install packages `llvm` and `llvm-libs` from the repository `extra`. They are currently of version 4.0.

```sh
$ sudo pacman -S llvm llvm-libs
```

#### OS X

Install package `llvm` with homebrew.

```sh
$ brew install llvm
```

#### Other platforms

On other platforms, you'll need to build LLVM yourself.
See the LLVM documentation:  
[Linux](http://llvm.org/docs/GettingStarted.html)  
[Windows](http://llvm.org/docs/GettingStartedVS.html)  
[Downloads](releases.llvm.org/download.html#4.0.0)  
Please note, that your package manager may provide some version of LLVM, even though it wouldn't be listed here.
As long as their versions are compatible (4.0), using them should be fine.

### Other dependencies

Make sure to update submodules before building

```sh
$ git submodule update --init
```

On Linux, you'll also need to install libuuid.
The library files are already installed on some distributions, but the header files may be missing.
Libuuid is not required on Windows or Mac.

```sh
# Debian-based
$ sudo apt-get install uuid-dev
```

### Actual building

#### Linux

```sh
# On the repository root folder
$ mkdir build
$ cd build
# Your LLVM installation directory may be different
$ cmake -G "Unix Makefiles" -DLLVM_DIR=/usr/lib/llvm-4.0 -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=OFF ..
# Parallel builds are recommended
$ make -j4
$ sudo make install
```

#### Windows

You can use the CMake GUI to create the MSVC project files.
Note that the tests don't support Windows.

## Contributing

See CONTRIBUTING.md

## Editor support

An Atom package for Varuna syntax highlighting can be found here: https://github.com/varuna-lang/atom-language-varuna

## Compiler documentation

Compiler documentation is autogenerated by Doxygen. Its build can be triggered with the script `scripts/doxygen.sh`
After that the docs can be found in HTML and LaTEX formats in the folder `doc`.

WORK IN PROGRESS.
The documentation is lacking in a multitude of sections.

### Compiler structure

Here is a simplified list of things the Varuna compiler does in order to create its final output.

Compiler entry point (main-function) is in `Main.cpp`, which creates a `CLI`-object and runs it.
`CLI` will parse command line arguments and create tasks for `Runner` accordingly.
`Runner` will invoke compiler parts through `Dispatcher`.

`Core` is the compiler frontend and it consists of `Lexer` and `Parser`.
It takes the source code and outputs an `AST`.

That `AST` is then consumed by `Codegen`, which visits the `AST` with `CodegenVisitor`.
This process produces LLVM IR.
This is then optimized with `Optimizer` and compiled into the user desired format (object code by default).

`util` has some general utilities for application-wide use.

## License

Varuna is published under the 3-Clause BSD License ("New/Modified BSD License"),
which is included in the file LICENSE.

Copyright (C) 2016-2017 Elias Kosunen

Varuna also uses various third-party software, see third-party subdirectories for details.
