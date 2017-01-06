#!/bin/bash

# Reformat the whole src/ folder
#find src/ -iname "*.h" -o -iname "*.cpp" | xargs clang-format -i -style=file

# Format current pending changes and
# write it to stdout
#git diff -U0 --no-color HEAD^ | clang-format-diff-3.9 -p1 -style=file

# Format current pending changes and apply them
git diff -U0 --no-color HEAD^ | clang-format-diff-3.9 -p1 -i -style=file -v
