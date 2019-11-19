// 資工三甲 林詠翔
// must to use -std=c++11 or higher version

#include "parser.h"

#include <cassert>

Parser::Parser(Lexicaler *lexer) { this->lexer = lexer; }

void Parser::setTokens(vector<TokenData> tokenString) {
  this->tokenString = tokenString;
  syntaxIndex.clear();
}

int Parser::matchInstruction() {
  // instruction

  // pseudo

  // BYTE X'F1'
  // BYTE C'EOF'
  // WORD X'FFFFFF'
}

int Parser::matchSyntax(vector<TokenData> tokenString) {
  // define grammar

  // has symbol
  // no symbol
}

void Parser::test() { assert(false); }
