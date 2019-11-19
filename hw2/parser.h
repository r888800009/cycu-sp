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
  int curIndex;
  vector<int> syntaxIndex;

  vector<TokenData> *tokenString;

  void testBeginAndEnd(vector<TokenData> &tokens, bool result);
  void testNest(int deep);

 public:
  Parser(Lexicaler *lexer);  // define Syntax

  void syntaxBegin(vector<TokenData> *tokenString);
  void syntaxEnd();  // return match or not

  bool matchDelimiter(char c);
  bool matchRegister(const string &reg);
  bool matchOP(int format);

  int matchInstruction();

  int matchSyntax(vector<TokenData>);
  void test();
};

#endif
