#!/bin/bash
./build.sh
echo |./a.out

rm tmpout -r
mkdir tmpout

cp $(find testfile -name '*.txt') tmpout
for f in tmpout/input00*.txt; do
  iconv -f UTF-8 -t BIG-5 $f > $f.tmp
  mv $f.tmp $f
done


echo $(find tmpout -name '*.txt') | ./a.out > /dev/null

diff tmpout testOut

