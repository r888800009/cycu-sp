// 資工三甲 林詠翔
// must to use -std=c++11 or higher version
#include "assembler.h"

#include <cassert>

using namespace std;

Assembler::Assembler() {
  // lexer = Lexicaler(); Maybe not necessary?

  // OPTAB
  // SYMTAB
  // LOCCTR
}

void Assembler::setFile(const string &filename) { this->filename = filename; }

void Assembler::pass1() {}
void Assembler::pass2() {
  // check undefine
}

void Assembler::assembling() {
  try {
    lexer.loadFile(filename);
    lexer.lexing();
  } catch (Lexicaler::Error e) {
    if (e == Lexicaler::loading_failure)
      ;
    else if (e == Lexicaler::saving_failure)
      ;
    else
      throw "Unknown Error!";
  } catch (LoadingTable::Error e) {
    if (e == LoadingTable::loading_failure)
      ;
    else
      throw "Unknown Error!";
  }

  pass1();
  pass2();
}

