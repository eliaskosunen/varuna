#!/bin/bash

if [ ! -d "build" ]
then
	mkdir build
fi
cd build

#cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug -DLLVM_DIR=/usr/lib/llvm-3.9/lib/cmake/llvm \
#    -DCMAKE_CXX_COMPILER=g++-6 -DCMAKE_C_COMPILER=gcc-6 ..
cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug -DLLVM_DIR=/usr/lib/llvm-3.9/lib/cmake/llvm \
    -DCMAKE_CXX_COMPILER=clang++-3.9 -DCMAKE_C_COMPILER=clang-3.9 ..
#cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release -DLLVM_DIR=/usr/lib/llvm-3.9/lib/cmake/llvm \
#    -DCMAKE_CXX_COMPILER=clang++-3.9 -DCMAKE_C_COMPILER=clang-3.9 ..
make -j4

cd ..
# Comment out if you don't have valgrind
# or don't want to use it
valgrind bin/tests
#bin/tests

# Build documentation
echo Building documentation...
./scripts/doxygen.sh
