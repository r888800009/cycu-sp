// 資工三甲 林詠翔
// must to use -std=c++11 or higher version

#ifndef _ASSEMBLER_H_
#define _ASSEMBLER_H_

#include "lexicaler.h"
#include "optab.h"

using namespace std;

class Assembler {
  Lexicaler lexer;
  OPTab optab;

  string filename;

  void pass1();
  void pass2();

  typedef struct Flag {
    bool n, i, x, b, p, e;
  } Flag;

  string genData(int data);
  string genFormat1(int opcode);
  string genFormat2(int opcode, int r1, int r2);
  string genFormat3(int opcode, Flag flag, int disp);
  string genFormat4(int opcode, Flag flag, int address);

 public:
  Assembler();
  void assembling();
  void setFile(const string &filename);
  void test();
  enum Error { syntax_error, duplicate_define_error, undefined_error };
};

#endif
