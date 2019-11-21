// 資工三甲 林詠翔
// must to use -std=c++11 or higher version

#include "parser.h"

#include <cassert>

#include "lexicaler.h"

Parser::Parser(Lexicaler *lexer, OPTab *optab) {
  this->lexer = lexer;
  this->optab = optab;
}

void Parser::setTokenString(vector<TokenData> *tokenString) {
  this->tokenString = tokenString;
}

void Parser::debug() {
  printTokenValue(matchData.symbol);
  cout << matchData.opcode << ", " << matchData.format << endl;

  cout << "n:" << matchData.flag.n << endl;
  cout << "i:" << matchData.flag.i << endl;
  cout << "x:" << matchData.flag.x << endl;
  cout << "b:" << matchData.flag.b << endl;
  cout << "p:" << matchData.flag.p << endl;
  cout << "e:" << matchData.flag.e << endl;

  cout.flush();
}

bool Parser::matchFormat1(const int r, int &l) {
  if (r >= tokenString->size()) return false;
  TokenData data = tokenString->at(r);

  string mnemonic = lexer->instructionTable.get(data);
  matchData.opcode = optab->getOPCode(mnemonic);
  matchData.format = optab->getFormat(mnemonic);
  l++;

  if (matchData.opcode != -1 && matchData.format == 1) return true;

  return false;
}

void Parser::testFmt1() {
  vector<TokenData> tokens;
  int l = 0;

  // fix
  tokens = {{1, 12}};
  setTokenString(&tokens);
  assert(matchFormat1(l = 0, l) == true && l != 0);
  assert(matchData.opcode == 0xc4);
  assert(matchData.format == 1);

  // null
  tokens = {};
  setTokenString(&tokens);
  assert(matchFormat1(l = 0, l) == false && l == 0);

  // op op without check end
  tokens = {{1, 12}, {1, 12}};
  setTokenString(&tokens);
  assert(matchFormat1(l = 0, l) == true && l != 0);

  // fmt2
  tokens = {{1, 3}};
  setTokenString(&tokens);
  assert(matchFormat1(l = 0, l) == false && l != 0);

  // fmt3,4
  tokens = {{1, 1}};
  setTokenString(&tokens);
  assert(matchFormat1(l = 0, l) == false && l != 0);
}

bool Parser::matchFormat2(const int r, int &l) {
  if (r + 2 == tokenString->size()) {
    TokenData data = tokenString->at(l = r);
    string mnemonic = lexer->instructionTable.get(data);
    l++;

    matchData.opcode = optab->getOPCode(mnemonic);
    matchData.format = optab->getFormat(mnemonic);

    if (matchData.opcode != -1 && matchData.format == 1) return true;

    // CLEAR r1
    // SVC n
    // TIXR r1

  } else if (r + 4 == tokenString->size()) {
    // SHIFTL r1, n
    // SHIFTR r1, n

    // other format2
  }
}

void Parser::testFmt2() {
  // CLEAR r1
  // SVC n
  // TIXR r1

  // SHIFTL r1, n
  // SHIFTR r1, n

  // other format2
}

bool Parser::matchFormat3(const int r, int &l) {}
void Parser::testFmt3() {}

bool Parser::matchFormat4(const int r, int &l) {}
void Parser::testFmt4() {}

bool Parser::matchInstruction(const int r, int &l) {
  int base = r;

  // instruction
  // format 1
  // i

  // format 2

  // format 3
  // format 4

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

  // has comment or no comment at lest
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

int Parser::matchRegister(int i) {
  TokenData data = tokenString->at(i);
  if (lexer->registerTable.get(data) == "")
    return -1;
  else
    return data.value - 1;
}

void Parser::testReg() {
  vector<TokenData> tokens;
  int l = 0;

  // has reg
  for (int i = 1; i <= 9; i++) {
    tokens = {{3, i}};
    setTokenString(&tokens);
    assert(matchRegister(0) == i - 1);
  }

  // not reg
  tokens = {{1, 1}};
  setTokenString(&tokens);
  assert(matchRegister(0) == -1);

  tokens = {{2, 1}};
  setTokenString(&tokens);
  assert(matchRegister(0) == -1);
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

  testReg();

  testFmt1();
  testFmt2();
  testFmt3();
  testFmt4();
}
