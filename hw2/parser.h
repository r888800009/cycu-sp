// 資工三甲 林詠翔
// must to use -std=c++11 or higher version

#ifndef _PARSER_H_
#define _PARSER_H_

#include <regex>
#include <vector>

#include "format.h"
#include "lexicaler.h"
#include "optab.h"
#include "table.h"

using namespace std;

class Parser {
  Lexicaler *lexer = nullptr;
  vector<TokenData> *tokenString;
  OPTab *optab;

  void testBeginAndEnd(vector<TokenData> &tokens, bool result);
  void debug();

 public:
  struct MatchData {
    TokenData symbol;
    int opcode, format;
    int op1, op2;
    Flag flag;
  } matchData;

  Parser(Lexicaler *lexer, OPTab *optab);

  void setTokenString(vector<TokenData> *tokenString);

  bool matchDelimiter(char c, int i);
  bool matchRegister(const string &reg, int i);
  bool matchOP(const string &mnemonic, int i);
  bool matchOP(int i);
  bool matchSymbol(int i);
  int matchRegister(int i);
  int matchN(int i);
  bool getOPData(int format, int i);

  // r; begin, l: end
  bool matchFormat1(const int r, int &l);
  bool matchFormat2(const int r, int &l);
  bool matchFormat3(const int r, int &l);
  bool matchFormat4(const int r, int &l);
  bool matchInstruction(const int r, int &l);

  int matchSyntax(vector<TokenData>);

  void dataClear();
  void test();
  void testReg();
  void testN();
  void testDelimiter();
  void testOp();

  void testFmt1();
  void testFmt2();
  void testFmt3();
  void testFmt4();
};

#endif
