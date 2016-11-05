#!/bin/bash

if [ ! -d "build" ]
then
	mkdir build
fi
cd build

cmake -G "Unix Makefiles" -DCMAKE_CXX_COMPILER=g++-5 ..
make

cd ..
./bin/tests
./build-docs.sh
