#!/bin/bash

#find src/ -iname "*.h" -o -iname "*.cpp" | xargs clang-format -i -style=file
git diff -U0 --no-color HEAD^ | clang-format-diff-3.9 -p1 -i -style=file -v
