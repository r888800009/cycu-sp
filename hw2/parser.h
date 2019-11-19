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
  vector<int> syntaxIndex;
  vector<TokenData> tokenString;

 public:
  Parser(Lexicaler *lexer);  // define Syntax

  void syntaxBegin();
  bool syntaxEnd();  // return match or not
  void setTokens(vector<TokenData>);

  bool matchDelimiter(char c);
  bool matchOP();
  int matchInstruction();

  int matchSyntax(vector<TokenData>);
  void test();
};

#endif
