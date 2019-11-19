// 資工三甲 林詠翔
// must to use -std=c++11 or higher version

#include "parser.h"

#include <cassert>

#include "lexicaler.h"

Parser::Parser(Lexicaler *lexer) { this->lexer = lexer; }

void Parser::setTokenString(vector<TokenData> *tokenString) {
  this->tokenString = tokenString;
}

int Parser::matchInstruction() {
  // instruction

  // pseudo

  // BYTE X'F1'
  // BYTE C'EOF'
  // WORD X'FFFFFF'
}

bool Parser::matchSymbol(int i) {
  if (i >= tokenString->size()) return false;
  TokenData data = tokenString->at(i);
  if (!lexer->symbolTable.exist(data)) return false;

  matchData.symbol = data;
  return true;
}

int Parser::matchSyntax(vector<TokenData> tokenString) {
  // define grammar

  // has symbol
  // no symbol
}

bool Parser::matchDelimiter(char c, int i) {
  if (i >= tokenString->size()) return false;
  return isTokenEqual(tokenString->at(i), lexer->delimiterTable.get(c));
}

bool Parser::matchRegister(const string &reg, int i) {
  if (i >= tokenString->size()) return false;
  return isTokenEqual(tokenString->at(i), lexer->registerTable.get(reg));
}

bool Parser::matchOP(int format, int i) { return false; }

void Parser::testBeginAndEnd(vector<TokenData> &tokens, bool result) {
  setTokenString(&tokens);
  bool syntax = matchDelimiter(',', 0) && matchDelimiter(',', 1) &&
                matchDelimiter(',', 2) && matchDelimiter(',', 3);
  assert(syntax == result);
}

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

  // delimiter
  // ,,,,
  tokens = {{4, 1}, {4, 1}, {4, 1}, {4, 1000}, {4, 100}};
  testBeginAndEnd(tokens, false);

  // match symbol
  lexer->symbolTable.put("test");  // define a symbol
  tokens = {{SYMBOL_TABLE, ('t' + 'e' + 's' + 't') % 100}};
  setTokenString(&tokens);
  assert(matchSymbol(0));
  assert(isTokenEqual(tokens[0], this->matchData.symbol));
}
