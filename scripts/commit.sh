#!/bin/bash

./bin/tests

./format.sh

git status

read -rsp $'Press any key to continue...\n' -n1 key

git add .
git commit
