#!/bin/bash

make build_all CC=${1:-gcc} CXX=${2:-g++}

if [[ $@ != *-notests* ]]
then
	./bin/test
fi

if [[ $@ != *-nohtml* ]]
then
	make html
fi
