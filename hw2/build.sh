#!/bin/bash

if [ -z $1]; then
  echo $(date) >> ./logging.txt
  echo debug mode
  g++ -J 4 *.cpp -D DEBUGING -g
  echo test
  ./a.out --debuging
else
  echo Release mode
  g++ *.cpp -D NDEBUG
fi
