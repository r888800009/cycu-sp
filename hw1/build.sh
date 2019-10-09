#!/bin/bash

if [ -z $1]; then
  echo debug mode
  g++ *.cpp -D DEBUGING
else
  echo Release mode
  g++ *.cpp -D NDEBUG
fi
