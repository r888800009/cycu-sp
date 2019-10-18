#!/bin/bash

if [ -z $1]; then
  echo $(date) >> ./logging.txt
  echo debug mode
  g++ *.cpp -D DEBUGING -g
  echo test
  ./a.out --debuging

  echo test files
  fileList='error1 SIC SICXE2 test2 MY SICXE1 test1'
  for filename in $fileList; do
    ./a.out ./TestFile/"$filename"_input.txt  >> ./logging.txt
    diff TestFile/"$filename"_output.txt output/"$filename"_output.txt
    if [ $? -eq 1 ]; then
      echo $filename error
    fi
  done
else
  echo Release mode
  g++ *.cpp -D NDEBUG
fi
