#!/bin/bash

mkdir -p build
cd build

cmake -G "Unix Makefiles" -DCMAKE_CXX_COMPILER=g++-5 ..
make

cd ..

if [[ $@ != *-notests* ]]
then
	./bin/tests
fi

if [[ $@ != *-nohtml* ]]
then
	make html
fi
