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
#include "symtab.h"

using namespace std;

class Assembler {
  Lexicaler lexer;
  Parser parser;
  OPTab optab;
  SymbolTable symtab;

  typedef struct Literal {
    TokenData literal;
    int address;
  } Literal;

  vector<Literal> littab;

  bool enableBase = false, sicxe = false;
  bool hasSTART = false, hasEND = false;
  stringstream printStream;

  fstream fin;
  string filename, saveName;
  int locationCounter = 0, curLocationCounter;
  int baseCounter = 0;

  string doPseudo(int pass);
  void genLiteral(int pass);
  string genInstruction(int pass);
  void pass(int pass);
  void matchPreSymbol(const string &line);
  string fill(int number, int byte);

  int memoryToAddr();
  bool isTokenMemory();
  int doEQU(int pass);

 private:
  vector<vector<TokenData>> lineBuffer;

  const int OFFSET_LINE = 6;
  const int OFFSET_LOCATION = 6;
  const int OFFSET_STATEMENT = 60;
  const int OFFSET_CODE = 10;

  string genData(int data);
  string genFormat1(int opcode);
  string genFormat2(int opcode, int r1, int r2);
  string genFormat3(int opcode, Flag flag, int disp);
  string genFormat4(int opcode, Flag flag, int address);

  Flag genAddressingTypeFlag();
  int flagToBinary(Flag flag);

  void printHeader();
  void printLine(int line, int loc, const string &statement,
                 const string &objcode);

 public:
  Assembler();
  void loadFile(const string &);
  void assembling();
  void setXE(bool sicxe);
  void test();
};

#endif
