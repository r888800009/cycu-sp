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
  bool sicxe = false;
  OPTab *optab;

  void testBeginAndEnd(vector<TokenData> &tokens, bool result);
  void debug();
  bool emptyLine;

 public:
  enum AddressingType {
    null_addressing,
    simple_addressing,
    indirect_addressing,
    immediate_addressing
  };

  enum Pseudo {
    START,
    END,
    BYTE,
    WORD,
    RESB,
    RESW,
    EQU,
    BASE,
    LTORG,
    NOT_PSEUDO
  };

  struct MatchData {
    TokenData preSymbol;
    struct StringData {
      TokenData value;
      unsigned int integer;
      string str;
      enum Type { integer_hex, integer_dec, string_data, null } type;
    } stringData;

    TokenData memory, literal;
    AddressingType addrType;
    bool x;

    Pseudo pseudo;

    // start
    unsigned int startMatch;
    TokenData baseMatch;
    TokenData resMatch;
    vector<TokenData> equMatch;

    int opcode, format;
    int op1, op2;

  } match;

  TokenData tmpSymbol;
  Parser(Lexicaler *lexer, OPTab *optab);

  void setTokenString(vector<TokenData> *tokenString);

  bool matchDelimiter(char c, int i);
  bool matchRegister(const string &reg, int i);
  bool matchOP(const string &mnemonic, int i);
  bool matchOP(int i);
  bool matchSymbol(int i);
  bool matchPrefixSymbol(int i);
  int matchRegister(int i);
  int matchN(int i);
  bool matchMemory(const int r);
  bool getOPData(int format, int i);
  void setXE(bool sicxe);
  bool isEmpty();

  // r; begin, l: end
  bool matchFormat1(const int r, int &l);
  bool matchFormat2(const int r, int &l);
  bool matchFormat3(const int r, int &l);
  bool matchFormat4(const int r, int &l);
  bool matchPseudo(const int r, int &l);
  bool matchPseudoToken(const string &pseudo, int i);
  bool matchConst(const int r, int &l, const string &pseudo, const int max,
                  Pseudo setPesudo);
  bool matchSIC(const int r, int &l);
  bool matchInstruction(const int r, int &l);
  bool matchString(const int r, int &l);
  // note: matchInteger would not check integer range
  // only check the size is less then 3 bytes
  bool matchIntegerHex(const int r, int &l);
  bool matchIntegerHex(int i);
  bool matchIntegerDec(int i);

  int matchSyntax(vector<TokenData>);

  void dataClear();
  void test();
  void testReg();
  void testN();
  void testDelimiter();
  void testInteger();
  void testString();
  void testMemory();
  void testOp();
  void testMode();
  void testPseudo();

  void testSTART();
  void testEND();
  void testBYTE();
  void testWORD();
  void testRESB();
  void testRESW();
  void testBASE();
  void testLTORG();
  void testEQU();
  void testEQUAdv();

  void testFmt1();
  void testFmt2();
  void testFmt3();
  void testFmt4();
};

#endif
