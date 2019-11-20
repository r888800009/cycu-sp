// 資工三甲 林詠翔
// must to use -std=c++11 or higher version

#ifndef _PARSER_H_
#define _PARSER_H_

#include <regex>
#include <vector>

#include "lexicaler.h"
#include "optab.h"
#include "table.h"

using namespace std;

class Parser {
  Lexicaler *lexer = nullptr;
  vector<TokenData> *tokenString;
  OPTab *optab;

  void testBeginAndEnd(vector<TokenData> &tokens, bool result);

 public:
  struct MatchData {
    TokenData symbol;
  } matchData;

  Parser(Lexicaler *lexer, OPTab *optab);

  void setTokenString(vector<TokenData> *tokenString);

  bool matchDelimiter(char c, int i);
  bool matchRegister(const string &reg, int i);
  bool matchOP(int format, int i);
  bool matchSymbol(int i);

  bool matchInstruction(int i);

  int matchSyntax(vector<TokenData>);
  void test();
};

#endif
