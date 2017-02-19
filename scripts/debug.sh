#!/bin/bash

#valgrind --leak-check=full --show-leak-kinds=all bin/varuna -logging=trace code.va
#valgrind --leak-check=full bin/varuna -logging=trace code.va
valgrind bin/varuna -logging=trace code.va -llvm-as=llvm-as-3.9 -llvm-llc=llc-3.9 -llvm-opt=opt-3.9 -emit=llvm-ir -o -
