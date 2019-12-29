#!/bin/bash

./build.sh

rm tmpout -r
mkdir tmpout
cp $(find testfile -name '*.txt') tmpout

echo $(find tmpout -name '*.txt') | ./a.out > /dev/null

diff tmpout testOut

