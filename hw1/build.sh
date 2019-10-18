#!/bin/bash

if [ -z $1]; then
  echo debug mode
  g++ *.cpp -D DEBUGING -g
  echo test
  ./a.out --debuging

  echo test files
  fileList='error1.txt test1.txt test2.txt'
  for filename in $fileList; do
    ./a.out ./TestFile/$filename >> ./logging.txt
    diff TestFile/$filename.output.txt output/$filename
    if [ $? -eq 1 ]; then
      echo $filename error
    fi
  done
else
  echo Release mode
  g++ *.cpp -D NDEBUG
fi
