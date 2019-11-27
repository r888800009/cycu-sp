// 資工三甲 林詠翔
// must to use -std=c++11 or higher version

#ifndef _ASSEMBLER_H_
#define _ASSEMBLER_H_

#include <iostream>
#include <sstream>

#include "format.h"
#include "lexicaler.h"
#include "optab.h"
#include "parser.h"

using namespace std;

class Assembler {
  Lexicaler lexer;
  Parser parser;
  OPTab optab;
  stringstream printStream;

  string filename;

  void pass1();
  void pass2();

 private:
  const int OFFSET_LINE = 6;
  const int OFFSET_LOCATION = 6;
  const int OFFSET_STATEMENT = 30;
  const int OFFSET_CODE = 10;

  string genData(int data);
  string genFormat1(int opcode);
  string genFormat2(int opcode, int r1, int r2);
  string genFormat3(int opcode, Flag flag, int disp);
  string genFormat4(int opcode, Flag flag, int address);

  int flagToBinary(Flag flag);

  void printHeader();
  void printLine(int line, int loc, const string &statement,
                 const string &objcode);

 public:
  Assembler();
  void assembling();
  void setFile(const string &filename);
  void test();
};

#endif
