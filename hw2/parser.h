// 資工三甲 林詠翔
// must to use -std=c++11 or higher version

#ifndef _PARSER_H_
#define _PARSER_H_

#include <regex>
#include <vector>

#include "table.h"

using namespace std;

class Parser {
 public:
  typedef class SyntaxData {
    int tableType;
    bool checkReg;
    regex reg;
    SyntaxData *next;

   public:
    SyntaxData(int type, bool enableRegex, regex reg);
    void append(SyntaxData nextSyntax);
  } SyntaxData;

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
