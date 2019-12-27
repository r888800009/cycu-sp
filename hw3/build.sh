#!/bin/bash
bison main.y -d -v
flex lexer.l
gcc *.c -lfl
