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

if [[ $@ == *-cov* ]]
then
	GCOV_EXE=gcov-5 # gcov and g++ versions must match
	OBJ_FILES_PATH=build/

	echo [GCOV]

	if [ ! -d "$OBJ_FILES_PATH" ]
	then
		echo "Object file path not found at "$OBJ_FILES_PATH
		exit 1
	fi

	for filename in `find src/ | egrep '\.cpp'`;
	do
		$GCOV_EXE -n -o $OBJ_FILES_PATH $filename > /dev/null;
	done
fi
