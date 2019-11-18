// 資工三甲 林詠翔
// must to use -std=c++11 or higher version

#ifndef _PARSER_H_
#define _PARSER_H_

#include <regex>
#include <vector>

#include "lexicaler.h"
#include "table.h"

using namespace std;

class SyntexList;

class SyntaxData {
 public:
  bool terminal = true;
  SyntaxData();
  virtual bool checkSyntax(const TokenData token, const Lexicaler &lexer,
                           const SyntexList &syntaxList) {
    return false;
  };
};

class Syntax {
  typedef vector<SyntaxData> TokenGroup;
  vector<TokenGroup> syntaxGroup;

 public:
  void append(TokenGroup);
  int match(vector<TokenData>);  // if match return  syntaxGroup index else
                                 // return -1
};

class SyntexList {
  vector<Syntax> list;
  map<string, int> identiferList;

 public:
  int append(string identifer, Syntax);
  int find(string identifer);
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
  string syntaxID;

 public:
  SyntaxSyntax(const string &syntaxID);
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
