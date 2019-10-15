#!/bin/bash

if [ -z $1]; then
  echo debug mode
  g++ *.cpp -D DEBUGING -g
  echo test
  ./a.out --debuging
else
  echo Release mode
  g++ *.cpp -D NDEBUG
fi
