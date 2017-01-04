#valgrind --leak-check=full --show-leak-kinds=all bin/varuna --logging trace code.va
#valgrind --leak-check=full bin/varuna --logging trace code.va
valgrind bin/varuna --logging trace code.va
