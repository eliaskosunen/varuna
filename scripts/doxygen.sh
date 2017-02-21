#!/bin/bash

mkdir -p doc
doxygen 1> doc/doxygen.log 2> doc/doxygen-error.log
