#!/bin/bash

bison -d -o parser.tab.cpp parser.y
flex -o lex.yy.cpp scanner.l

g++ -std=c++11 main.cpp parser.tab.cpp lex.yy.cpp -o compiler
