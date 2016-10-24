#!/bin/bash

make build_all CC=${1:-gcc} CXX=${2:-g++}
./bin/test

if [[ $@ != *-nohtml* ]]
then
	make html
fi