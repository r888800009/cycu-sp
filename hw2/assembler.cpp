// 資工三甲 林詠翔
// must to use -std=c++11 or higher version
#include "assembler.h"

#include <cassert>
#include <iomanip>
#include <sstream>

using namespace std;

Assembler::Assembler() {
  // lexer = Lexicaler(); Maybe not necessary?

  // OPTAB
  // SYMTAB
  // LOCCTR
}

string Assembler::genFormat1(int opcode) {
  stringstream stream;

  stream << std::noshowbase << setfill('0') << setw(2) << uppercase << std::hex
         << opcode;

  return stream.str();
}

string Assembler::genFormat2(int opcode, int r1, int r2) {
  stringstream stream;

  stream << std::noshowbase << setfill('0') << setw(2) << uppercase << std::hex
         << opcode << r1 << r2;

  return stream.str();
}

string Assembler::genFormat3(int opcode, Flag flag, int disp) {}
string Assembler::genFormat4(int opcode, Flag flag, int address) {}

void Assembler::test() {
  string code;
  // format1
  code = genFormat1(0xab);
  assert(code == "AB" && code.length() == 2);  // 2 nibble

  code = genFormat1(0x00);
  assert(code == "00" && code.length() == 2);  // 2 nibble

  // format2
  code = genFormat2(0xAB, 0xC, 0xD);
  assert(code == "ABCD" && code.length() == 4);  // 4 nibble

  code = genFormat2(0x00, 0x4, 0x9);
  assert(code == "0049" && code.length() == 4);  // 4 nibble

  code = genFormat2(0x00, 0x0, 0x0);
  assert(code == "0000" && code.length() == 4);  // 4 nibble

  // format3
  code = genFormat3(0b111111, {1, 1, 0, 0, 0, 0}, 0x0);
  assert(code == "FF0000" && code.length() == 6);  // 6 nibble

  // format4
  code = genFormat4(0b111111, {1, 1, 0, 0, 0, 0}, 0x0);
  assert(code == "FF000000" && code.length() == 8);  // 8 nibble
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

