#!/bin/bash

#valgrind --leak-check=full --show-leak-kinds=all bin/varuna --logging=info code.va -emit=llvm-ir -o -
valgrind --leak-check=full bin/varuna -logging=info code.va -emit=llvm-ir -o -
#valgrind bin/varuna -logging=info code.va -emit=llvm-ir -o -
