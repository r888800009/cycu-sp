#!/bin/bash

if [ -z $1]; then
  echo debug mode
  g++ *.cpp -D DEBUGING -g
  echo test
  ./a.out --debuging

  echo test files
  fileList='error1.txt test1.txt test2.txt'
  rm test-temp -r
  mkdir test-temp
  for filename in $fileList; do
    ./a.out ./TestFile/$filename > test-temp/$filename
    drophead=12
    diff <(tail -n +$drophead test-temp/$filename) <(tail -n +$drophead output/$filename)
    if [ $? -eq 1 ]; then
      echo $filename error
    fi
  done
else
  echo Release mode
  g++ *.cpp -D NDEBUG
fi
