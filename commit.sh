#!/bin/bash

./bin/tests

git diff -U0 --no-color HEAD^ | clang-format-diff-3.9 -p1 -style=file -sort-includes

git status
git add .
git commit
