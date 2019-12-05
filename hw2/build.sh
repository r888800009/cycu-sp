#!/bin/bash

if [ -z $1]; then
  echo $(date) >> ./logging.txt
  echo debug mode
  g++ -J 4 *.cpp -D DEBUGING -g
  echo test
  ./a.out --debuging
  cat fileTest/testList.txt|./a.out
  diff -r fileTest fileTestPre
  echo get no test
  diff -r fileTest fileTestPre | grep '_output.txt'
else
  echo Release mode
  g++ *.cpp -D NDEBUG
fi
