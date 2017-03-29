#!/bin/bash

./bin/tests

./scripts/format.sh

git add .
git status

read -rsp $'Press any key to continue...\n' -n1 key

git commit
