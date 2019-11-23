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
  cout << endl << "token string size:" << tokenString->size() << endl;
  cout << matchData.opcode << ", " << matchData.format << endl;

  cout << "op1" << matchData.op1 << endl;
  cout << "op2" << matchData.op1 << endl;

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
  dataClear();
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
  TokenData data;
  l = r;
  if (r >= tokenString->size()) return false;

  // get opcode
  data = tokenString->at(l);
  string mnemonic = lexer->instructionTable.get(data);

  matchData.opcode = optab->getOPCode(mnemonic);
  matchData.format = optab->getFormat(mnemonic);

  if (matchData.opcode == -1 && matchData.format != 2) return false;

  if (r + 2 >= tokenString->size()) {
    // CLEAR {r1}, TIXR {r1}
    bool cond1 = (matchOP("CLEAR", l) || matchOP("TIXR", l)) &&
                 ((matchData.op1 = matchRegister(l + 1)) != -1);
    // SVC {n}
    bool cond2 = matchOP("SVC", l) && ((matchData.op1 = matchN(l + 1)) != -1);

    l += 2;
    matchData.op2 = 0;

    return cond1 || cond2;
  } else if (r + 4 >= tokenString->size()) {
    // SHIFTL r1, n
    // SHIFTR r1, n

    // other format2
  }

  return false;
}

void Parser::dataClear() {
  matchData.symbol = {-1, -1};
  matchData.opcode = -1;
  matchData.format = -1;
  matchData.op1 = -1;
  matchData.op2 = -1;
  matchData.flag = {0, 0, 0, 0, 0};
}

void Parser::testFmt2() {
  vector<TokenData> tokens;
  int l = 0;

  // null
  tokens = {};
  setTokenString(&tokens);
  assert(matchFormat2(l = 0, l) == false);

  // CLEAR r1, TIXR r1
  dataClear();
  tokens = {{1, 5}, {3, 8}};
  setTokenString(&tokens);
  assert(matchFormat2(l = 0, l) && l == 2);
  assert(matchData.opcode == 0xb4);
  assert(matchData.format == 2);
  assert(matchData.op1 == 8 - 1);
  assert(matchData.op2 == 0);

  // error ClEAR n
  tokens = {{1, 5}, lexer->integerTable.put(to_string(2))};
  setTokenString(&tokens);
  assert(matchFormat2(l = 0, l) == false);
  lexer->reset();

  // SVC n
  dataClear();
  tokens = {{1, 54}, lexer->integerTable.put(to_string(5))};
  setTokenString(&tokens);
  assert(matchFormat2(l = 0, l) && l == 2);
  assert(matchData.opcode == 0xb0);
  assert(matchData.format == 2);
  assert(matchData.op1 == 5);
  assert(matchData.op2 == 0);
  lexer->reset();

  // error SVC r1
  tokens = {{1, 54}, {3, 1}};
  setTokenString(&tokens);
  assert(matchFormat2(l = 0, l) == false);

  // SHIFTL r1, n
  dataClear();
  tokens = {{1, 37}, {3, 2}, {4, 1}, lexer->integerTable.put(to_string(5))};
  setTokenString(&tokens);
  assert(matchFormat2(l = 0, l) && l == 4);
  assert(matchData.opcode == 0xa4);
  assert(matchData.format == 2);
  assert(matchData.op1 == 1);
  assert(matchData.op2 == 4);
  lexer->reset();

  // SHIFTR r1, n
  dataClear();
  tokens = {{1, 38}, {3, 2}, {4, 1}, lexer->integerTable.put(to_string(5))};
  setTokenString(&tokens);
  assert(matchFormat2(l = 0, l) && l == 4);
  assert(matchData.opcode == 0xa8);
  lexer->reset();

  // error SHIFTL r1, r2
  tokens = {{1, 38}, {3, 2}, {4, 1}, {3, 1}};
  setTokenString(&tokens);
  assert(matchFormat2(l = 0, l) == false);

  // other format2
  // op r1,r2
  dataClear();
  tokens = {{1, 3}, {3, 2}, {4, 1}, {3, 1}};
  setTokenString(&tokens);
  assert(matchFormat2(l = 0, l) && l == 4);
  assert(matchData.opcode == 0x90);
  assert(matchData.format == 2);
  assert(matchData.op1 == 1);
  assert(matchData.op2 == 0);

  // error op r1, n
  dataClear();
  tokens = {{1, 3}, {3, 2}, {4, 1}, lexer->integerTable.put(to_string(5))};
  setTokenString(&tokens);
  assert(matchFormat2(l = 0, l) == false);
  lexer->reset();

  // not format 2
  dataClear();
  tokens = {{2, 3}, {3, 2}, {4, 1}, {3, 1}};
  setTokenString(&tokens);
  assert(matchFormat2(l = 0, l) == false);
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

bool Parser::matchOP(const string &mnemonic, int i) {
  if (i >= tokenString->size()) return false;
  return isTokenEqual(tokenString->at(i),
                      lexer->instructionTable.get(mnemonic));
}

bool Parser::matchOP(int i) {
  if (i >= tokenString->size()) return false;
  return lexer->instructionTable.get(tokenString->at(i)) != "";
}

void Parser::testOp() {
  vector<TokenData> tokens;

  // null
  tokens = {};
  setTokenString(&tokens);
  assert(matchOP("ADD", 0) == false);
  assert(matchOP(0) == false);

  // match
  tokens = {{1, 1}};
  setTokenString(&tokens);
  assert(matchOP("ADD", 0));
  assert(matchOP(0));

  // not match
  tokens = {{1, 2}};
  setTokenString(&tokens);
  assert(matchOP("ADD", 0) == false);

  // another table
  tokens = {{2, 1}};
  setTokenString(&tokens);
  assert(matchOP("ADD", 0) == false);
  assert(matchOP(0) == false);
}

void Parser::testBeginAndEnd(vector<TokenData> &tokens, bool result) {
  setTokenString(&tokens);
  bool syntax = matchDelimiter(',', 0) && matchDelimiter(',', 1) &&
                matchDelimiter(',', 2) && matchDelimiter(',', 3);
  assert(syntax == result);
}

int Parser::matchN(int i) {
  if (i >= tokenString->size()) return -1;

  TokenData data = tokenString->at(i);
  string num = lexer->integerTable.get(data);
  if (num == "") return -1;

  // check dec
  if (!regex_match(num, regex("[0-9]+"))) return -1;

  // check range 1 ~ 16
  int result = stoi(num);

  if (1 <= result && result <= 16) return result;

  return -1;
}

void Parser::testN() {
  vector<TokenData> tokens;
  string tmp;

  // null
  tokens = {};
  setTokenString(&tokens);
  assert(matchN(0) == -1);

  // test 1 ~ 16
  for (int i = 1; i <= 16; i++) {
    tokens = {lexer->integerTable.put(to_string(i))};
    setTokenString(&tokens);
    assert(matchN(0) == i);

    lexer->reset();
  }

  // not 1 ~ 16
  for (int i = 17; i <= 50; i++) {
    tokens = {lexer->integerTable.put(to_string(i))};
    setTokenString(&tokens);
    assert(matchN(0) == -1);

    lexer->reset();
  }

  // hex
  tokens = {lexer->integerTable.put("A")};
  setTokenString(&tokens);
  assert(matchN(0) == -1);
  lexer->reset();

  // not integer
  tokens = {{1, 1}};
  setTokenString(&tokens);
  assert(matchN(0) == -1);
}

int Parser::matchRegister(int i) {
  if (i >= tokenString->size()) return -1;
  TokenData data = tokenString->at(i);
  if (lexer->registerTable.get(data) == "")
    return -1;
  else
    return data.value - 1;
}

void Parser::testReg() {
  vector<TokenData> tokens;

  // null
  tokens = {};
  setTokenString(&tokens);
  assert(matchRegister(0) == -1);

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

void Parser::testDelimiter() {
  vector<TokenData> tokens;

  // null
  tokens = {};
  setTokenString(&tokens);
  assert(matchDelimiter(',', 0) == false);

  // delimiter
  tokens = {{4, 13}};
  setTokenString(&tokens);
  assert(matchDelimiter('@', 0) == true);

  // another delimiter
  tokens = {{4, 13}};
  setTokenString(&tokens);
  assert(matchDelimiter(',', 0) == false);

  // out of table
  tokens = {{4, 14}};
  setTokenString(&tokens);
  assert(matchDelimiter(',', 0) == false);

  // not delimiter
  tokens = {{1, 1}};
  setTokenString(&tokens);
  assert(matchDelimiter(',', 0) == false);
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
  testDelimiter();
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
  testN();
  testOp();

  testFmt1();
  testFmt2();
  testFmt3();
  testFmt4();
}
