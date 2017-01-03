#!/bin/bash

if [ ! -d "html" ]
then
	mkdir html
fi

doxygen 1> html/doxygen.log 2> html/doxygen-error.log
