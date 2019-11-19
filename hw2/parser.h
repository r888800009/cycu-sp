// 資工三甲 林詠翔
// must to use -std=c++11 or higher version

#ifndef _PARSER_H_
#define _PARSER_H_

#include <regex>
#include <vector>

#include "lexicaler.h"
#include "table.h"

using namespace std;

class Parser {
  Lexicaler *lexer = nullptr;
  vector<TokenData> *tokenString;

  void testBeginAndEnd(vector<TokenData> &tokens, bool result);

 public:
  Parser(Lexicaler *lexer);  // define Syntax

  void setTokenString(vector<TokenData> *tokenString);

  bool matchDelimiter(char c, int i);
  bool matchRegister(const string &reg, int i);
  bool matchOP(int format, int i);

  int matchInstruction();

  int matchSyntax(vector<TokenData>);
  void test();
};

#endif
