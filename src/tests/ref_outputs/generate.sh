#!/bin/bash

for filename in ../inputs/*.va; do
    name=${filename##*/}
    base=${name%.va}
    if [[ ${base} == *module* ]]; then
        ../../../bin/varuna ${filename} -o ${base}.ll -O0 -emit=llvm-ir -strip-debug -strip-source-filename
        ../../../bin/varuna ${filename} -o ${base}_opt.ll -O3 -emit=llvm-ir -strip-debug -strip-source-filename
    else
        ../../../bin/varuna ${filename} -o ${base}.ll -O0 -emit=llvm-ir -no-module -strip-debug -strip-source-filename
        ../../../bin/varuna ${filename} -o ${base}_opt.ll -O3 -emit=llvm-ir -no-module -strip-debug -strip-source-filename
    fi
done
