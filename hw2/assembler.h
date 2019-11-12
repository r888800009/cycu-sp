// 資工三甲 林詠翔
// must to use -std=c++11 or higher version

#ifndef _ASSEMBLER_H_
#define _ASSEMBLER_H_
#include "lexicaler.h"

using namespace std;

class Assembler {
  Lexicaler lexer;
  string filename;

  void pass1();
  void pass2();

 public:
  Assembler();
  void assembling();
  void setFile(const string &filename);
  enum Error { syntax_error, duplicate_define_error, undefined_error };
};

#endif
