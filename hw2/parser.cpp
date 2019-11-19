// 資工三甲 林詠翔
// must to use -std=c++11 or higher version

#include "parser.h"

#include <cassert>

#include "lexicaler.h"

Parser::Parser(Lexicaler *lexer) { this->lexer = lexer; }

void Parser::syntaxBegin(vector<TokenData> *tokenString) {
  this->tokenString = tokenString;
  syntaxIndex.clear();
  curIndex = 0;
  syntaxIndex.push_back(curIndex);
}

void Parser::syntaxEnd() { syntaxIndex.pop_back(); }

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

bool Parser::matchDelimiter(char c) {
  if (curIndex >= tokenString->size()) return false;

  TokenData d1 = tokenString->at(curIndex);
  TokenData d2 = lexer->delimiterTable.get(c);
  curIndex++;
  if (isTokenEqual(d1, d2)) return true;

  return false;
}

bool Parser::matchRegister(const string &reg) { return false; }
bool Parser::matchOP(int format) { return false; }

void Parser::testBeginAndEnd(vector<TokenData> &tokens, bool result) {
  // cout << tokens.size() << endl;
  syntaxBegin(&tokens);
  bool syntax = matchDelimiter(',') && matchDelimiter(',') &&
                matchDelimiter(',') && matchDelimiter(',');
  assert(syntax == result);
  syntaxEnd();
}

void Parser::testNest(int deep) {}

void Parser::test() {
  // begin and end
  vector<TokenData> tokens = {{4, 1}, {4, 1}, {4, 1}, {4, 1}};
  testBeginAndEnd(tokens, true);

  tokens = {{4, 1}, {4, 1}, {4, 1}};
  testBeginAndEnd(tokens, false);

  tokens = {};
  testBeginAndEnd(tokens, false);

  tokens = {{4, 1}, {4, 1}, {4, 1}, {4, 1}, {4, 1}};
  testBeginAndEnd(tokens, true);

  tokens = {{1, 1}, {4, 1}, {4, 1}, {4, 1}, {4, 1}};
  testBeginAndEnd(tokens, false);

  // nest Syntax

  // delimiter
  // ,,,,
  tokens = {{4, 1}, {4, 1}, {4, 1}, {4, 1000}, {4, 100}};
  testBeginAndEnd(tokens, false);
}
