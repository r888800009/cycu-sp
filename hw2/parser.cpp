// 資工三甲 林詠翔
// must to use -std=c++11 or higher version

#include "parser.h"

Parser::SyntaxData::SyntaxData(int type, bool enableRegex, regex reg) {}

Parser::Parser() {  // define Syntax
  // define root syntax
  rootSyntax = syntexList[0];

  // BYTE X'F1'

  // BYTE C'EOF'
}

void Parser::test() {}
