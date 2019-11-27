// 資工三甲 林詠翔
// must to use -std=c++11 or higher version
#include "assembler.h"

#include <cassert>
#include <iomanip>
#include <sstream>

#include "error.h"
#include "parser.h"

using namespace std;

Assembler::Assembler() : parser(&lexer, &optab) {
  // lexer = Lexicaler(); Maybe not necessary?
  optab.loadOPTab("OPtable.tsv");  // OPTAB

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

int Assembler::flagToBinary(Flag flag) {
  int result = 0;

  result |= (flag.n) << 5;
  result |= (flag.i) << 4;
  result |= (flag.x) << 3;
  result |= (flag.b) << 2;
  result |= (flag.p) << 1;
  result |= (flag.e) << 0;

  return result;
}

string Assembler::genFormat3(int opcode, Flag flag, int disp) {
  stringstream stream;
  int result = 0;

  result |= (0b11111100 & opcode) << (12 - 2) + 6;
  result |= flagToBinary(flag) << 12;
  result |= (disp & 0b111111111111);

  stream << std::noshowbase << setfill('0') << setw(6) << uppercase << std::hex
         << result;

  return stream.str();
}

string Assembler::genFormat4(int opcode, Flag flag, int address) {
  stringstream stream;
  int result = 0;

  result |= (0b11111100 & opcode) << (20 - 2) + 6;
  result |= flagToBinary(flag) << 20;
  result |= (address & 0b11111111111111111111);

  stream << std::noshowbase << setfill('0') << setw(8) << uppercase << std::hex
         << result;

  return stream.str();
}

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
  code = genFormat3(0b11111100, {1, 1, 0, 0, 0, 0}, 0x0);
  assert(code == "FF0000" && code.length() == 6);  // 6 nibble

  code = genFormat3(0b11110000, {0, 0, 1, 1, 1, 1}, 0b111111111111);
  assert(code == "F0FFFF" && code.length() == 6);  // 6 nibble

  code = genFormat3(0b00000000, {0, 0, 1, 1, 1, 1}, 0b111111111111);
  assert(code == "00FFFF" && code.length() == 6);  // 6 nibble

  code = genFormat3(0b00000000, {0, 0, 1, 1, 1, 1}, 0b111100001111);
  assert(code == "00FF0F" && code.length() == 6);  // 6 nibble

  // add sic
  code = genFormat3(0x18, {0, 0, 1, 1, 1, 1}, 0b111100001111);
  assert(code == "18FF0F" && code.length() == 6);  // 6 nibble

  // format4
  code = genFormat4(0b11111100, {1, 1, 0, 0, 0, 0}, 0x0);
  assert(code == "FF000000" && code.length() == 8);  // 8 nibble

  code = genFormat4(0b11111100, {1, 1, 0, 0, 0, 0}, 0b11111111111111111111);
  assert(code == "FF0FFFFF" && code.length() == 8);  // 8 nibble

  code = genFormat4(0b00000000, {0, 0, 1, 1, 1, 1}, 0b11111111111111111111);
  assert(code == "00FFFFFF" && code.length() == 8);  // 8 nibble

  code = genFormat4(0b00000000, {0, 0, 1, 1, 1, 1}, 0b11111111111100001111);
  assert(code == "00FFFF0F" && code.length() == 8);  // 8 nibble

  // hello world

  printHeader();
  printLine(0, 0, "COPY  START 100", "");
  printLine(5, 1000, "COPY  START 100", "");
  printLine(10, 1000, "FIRST STL RETADR", "141033");
  cout << printStream.str();
}

void Assembler::setXE(bool sicxe) {
  sicxe = sicxe;
  parser.setXE(sicxe);
}

void Assembler::printHeader() {
  printStream << setw(OFFSET_LINE) << left << "Line" << setw(OFFSET_LOCATION)
              << left << "  Loc" << setw(OFFSET_STATEMENT) << left
              << "Source Statement" << setw(OFFSET_CODE) << left
              << "Object code" << endl;
  printStream << endl;
}

void Assembler::printLine(int line, int loc, const string &statement,
                          const string &objcode) {
  stringstream ss;
  ss << setw(4) << setfill('0') << loc;
  printStream << setw(OFFSET_LINE) << right << line << setw(OFFSET_LOCATION)
              << right << ss.str() << setfill(' ') << setw(OFFSET_STATEMENT)
              << left << (" " + statement) << setw(OFFSET_CODE) << left
              << objcode << endl;
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
  } catch (Error::IOError e) {
    if (e == Error::loading_failure)
      ;
    else if (e == Error::saving_failure)
      ;
    else
      throw "Unknown Error!";
  } catch (Error::IOError e) {
    if (e == Error::loading_failure)
      ;
    else
      throw "Unknown Error!";
  }

  pass1();
  pass2();
}

