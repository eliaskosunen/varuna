#!/bin/bash

if [ ! -d "build" ]
then
	mkdir build
fi
cd build

#cmake -G "Unix Makefiles" -DCMAKE_CXX_COMPILER=g++-6 -DCMAKE_C_COMPILER=gcc-6 -DLLVM_DIR=/usr/lib/llvm-3.9/lib/cmake/llvm ..
#cmake -G "Unix Makefiles" -DCMAKE_CXX_COMPILER=clang++-3.9 -DCMAKE_C_COMPILER=clang-3.9 -DLLVM_DIR=/usr/lib/llvm-3.9/lib/cmake/llvm ..
cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=clang++-3.9 -DCMAKE_C_COMPILER=clang-3.9 -DLLVM_DIR=/usr/lib/llvm-3.9/lib/cmake/llvm ..
make -j4

cd ..
# Comment out if you don't have valgrind
# or don't want to use it
valgrind bin/tests
#bin/tests
./build-docs.sh
