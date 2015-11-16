#!/bin/bash

#Colors:
RED='\033[0;31m'
BLU='\033[0;34m'
GRN='\033[0;32m'
YEL='\033[1;33m'
NC='\033[0m'

function run_single {
    printf "  ${BLU}[INFO] At test $1\n${NC}"
    ./ToONP "$(< tests/$1.in)" >"tests/_tmp"
    if !(diff -bq tests/$1.out tests/_tmp)
    then
            printf "    ${RED}[ERR] Your stdout differs!\n${NC}";
    else
            printf "    ${GRN}[OK] Stdout ok.\n${NC}"
    fi
    rm -rf "tests/_tmp"
}

for i in $(cat tests/testlist.txt); do
    run_single $i;
done;
