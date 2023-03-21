#!/bin/bash -e

topdir=$(realpath $(dirname $0)/..)
test_afl=${topdir}/build/test-afl
valgrind_opt="--show-leak-kinds=all --leak-check=full --error-exitcode=1 -q -v"

test_crash() {
        local input=$1
        local out

        if ! out=$(cat ${input} | valgrind ${valgrind_opt} ${test_afl} 2>&1); then
                echo -e "\n${out}"
                return 1
        else
                echo -n .
        fi
}

for i in $(ls ${topdir}/build/input/*); do
        test_crash $i
done
for i in $(ls ${topdir}/build/default/crashes/id*); do
        test_crash $i
done
for i in $(ls ${topdir}/build/afl/*/crashes/id*); do
        test_crash $i
done
echo -e "\nALL OK"