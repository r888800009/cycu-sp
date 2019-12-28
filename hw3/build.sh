#!/bin/bash
bison main.y -d -v -o main.tab.cpp
flex lexer.l
gcc -c ./lex.yy.c -o lex.yy.o
g++ lex.yy.o *.cpp -lfl -DDEEBUG
