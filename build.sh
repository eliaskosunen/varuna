#!/bin/bash

if [ ! -d "build" ]
then
	mkdir build
fi
cd build

#cmake -G "Unix Makefiles" -DCMAKE_CXX_COMPILER=g++-5 -DCMAKE_C_COMPILER=gcc-5 ..
cmake -G "Unix Makefiles" -DCMAKE_CXX_COMPILER=clang++-3.8 -DCMAKE_C_COMPILER=clang-3.8 ..
make -j4

cd ..
# Comment out if you don't have valgrind
# or don't want to use it
valgrind bin/tests
#bin/tests
./build-docs.sh
