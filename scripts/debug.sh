#!/bin/bash

#valgrind --leak-check=full --show-leak-kinds=all bin/varuna -logging=trace -j=2 code.va
#valgrind --leak-check=full bin/varuna -logging=trace -j=2 code.va
valgrind bin/varuna -logging=trace -j=2 code.va
