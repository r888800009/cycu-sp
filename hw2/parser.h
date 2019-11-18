// 資工三甲 林詠翔
// must to use -std=c++11 or higher version

#ifndef _PARSER_H_
#define _PARSER_H_

#include <regex>
#include <vector>

#include "lexicaler.h"
#include "table.h"

using namespace std;
class SyntaxData {
 public:
  bool terminal = true;
  SyntaxData();
  virtual bool checkSyntax(const TokenData token, const Lexicaler &lexer) {
    return false;
  };
};

class SyntaxInstruction : SyntaxData {};

class SyntaxPseudo : SyntaxData {};

class SyntaxDelimiter : SyntaxData {
  char delimiter;

 public:
  SyntaxDelimiter(char delimiter);
};

class SyntaxRegister : SyntaxData {};

class SyntaxSymbol : SyntaxData {};
class SyntaxInteger : SyntaxData {};
class SyntaxString : SyntaxData {};

class SyntaxSyntax : SyntaxData {
  int syntaxID;

 public:
  SyntaxSyntax(int syntaxID);
};

class Parser {
 public:
  typedef vector<SyntaxData> Syntex;

 private:
  vector<Syntex> syntexList;
  Syntex &rootSyntax = syntexList[0];

 public:
  Parser();  // define Syntax

  int matchSyntax(vector<TokenData>);
  void test();
};

#endif
