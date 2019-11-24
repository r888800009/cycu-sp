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
  printTokenValue(match.symbol);
  cout << endl << "token string size:" << tokenString->size() << endl;
  cout << match.opcode << ", " << match.format << endl;

  cout << "op1" << match.op1 << endl;
  cout << "op2" << match.op1 << endl;

  cout << "n:" << match.flag.n << endl;
  cout << "i:" << match.flag.i << endl;
  cout << "x:" << match.flag.x << endl;
  cout << "b:" << match.flag.b << endl;
  cout << "p:" << match.flag.p << endl;
  cout << "e:" << match.flag.e << endl;

  cout.flush();
}

bool Parser::matchFormat1(const int r, int &l) {
  if (r >= tokenString->size()) return false;
  if (getOPData(1, l)) {
    l++;
    return true;
  }

  return false;
}

void Parser::testFmt1() {
  vector<TokenData> tokens;
  int l = 0;

  // fix
  tokens = {{1, 12}};
  dataClear();
  setTokenString(&tokens);
  assert(matchFormat1(l = 0, l) && l != 0);
  assert(match.opcode == 0xc4);
  assert(match.format == 1);

  // null
  tokens = {};
  setTokenString(&tokens);
  assert(matchFormat1(l = 0, l) == false);

  // op op without check end
  tokens = {{1, 12}, {1, 12}};
  setTokenString(&tokens);
  assert(matchFormat1(l = 0, l) && l != 0);

  // fmt2
  tokens = {{1, 3}};
  setTokenString(&tokens);
  assert(matchFormat1(l = 0, l) == false);

  // fmt3,4
  tokens = {{1, 1}};
  setTokenString(&tokens);
  assert(matchFormat1(l = 0, l) == false);
}

bool Parser::matchFormat2(const int r, int &l) {
  TokenData data;
  l = r;
  if (r >= tokenString->size()) return false;

  // get opcode
  if (!getOPData(2, l)) return false;
  if (r + 3 < tokenString->size()) {
    int r1 = matchRegister(l + 1), r2 = matchRegister(l + 3);
    int n1 = matchN(l + 3);

    if (matchOP("SHIFTL", l) || matchOP("SHIFTR", l)) {
      if (r1 != -1 && matchDelimiter(',', l + 2) && n1 != -1) {
        // SHIFTL {r1}, {n} or SHIFTR {r1}, {n}
        match.op1 = r1;
        match.op2 = n1 - 1;

        l += 4;
        return true;
      }
    } else if (matchOP(l)) {
      if (r1 != -1 && matchDelimiter(',', l + 2) && r2 != -1) {
        // other format2
        match.op1 = r1;
        match.op2 = r2;

        l += 4;
        return true;
      }
    }
  } else if (r + 1 < tokenString->size()) {
    // CLEAR {r1}, TIXR {r1}
    bool cond1 = (matchOP("CLEAR", l) || matchOP("TIXR", l)) &&
                 ((match.op1 = matchRegister(l + 1)) != -1);
    // SVC {n}
    bool cond2 = matchOP("SVC", l) && ((match.op1 = matchN(l + 1)) != -1);

    l += 2;
    match.op2 = 0;

    return cond1 || cond2;
  }

  return false;
}

void Parser::dataClear() {
  match.symbol = {-1, -1};
  match.opcode = -1;
  match.format = -1;
  match.op1 = -1;
  match.op2 = -1;
  match.flag = {0, 0, 0, 0, 0};

  match.stringData.value = {-1, -1};
  match.stringData.type = MatchData::StringData::null;
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
  assert(match.opcode == 0xb4);
  assert(match.format == 2);
  assert(match.op1 == 8 - 1);
  assert(match.op2 == 0);

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
  assert(match.opcode == 0xb0);
  assert(match.format == 2);
  assert(match.op1 == 5);
  assert(match.op2 == 0);
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
  assert(match.opcode == 0xa4);
  assert(match.format == 2);
  assert(match.op1 == 1);
  assert(match.op2 == 4);
  lexer->reset();

  // SHIFTR r1, n
  dataClear();
  tokens = {{1, 38}, {3, 2}, {4, 1}, lexer->integerTable.put(to_string(5))};
  setTokenString(&tokens);
  assert(matchFormat2(l = 0, l) && l == 4);
  assert(match.opcode == 0xa8);
  lexer->reset();

  // test size
  dataClear();
  tokens = {{1, 38}, {3, 2}, {4, 1}, lexer->integerTable.put(to_string(5)),
            {1, 1},  {1, 1}};
  setTokenString(&tokens);
  assert(matchFormat2(l = 0, l) && l == 4);
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
  assert(match.opcode == 0x90);
  assert(match.format == 2);
  assert(match.op1 == 1);
  assert(match.op2 == 0);

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

  dataClear();
  tokens = {{1, 1}, {3, 2}, {4, 1}, {3, 1}};
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

  match.symbol = data;
  return true;
}

int Parser::matchSyntax(vector<TokenData> tokenString) {
  // define grammar

  // has comment or no comment at lest
  // has symbol
  // no symbol
}

bool Parser::matchString(const int r, int &l) {
  int size = 3;
  TokenData data;
  if (r + size - 1 < tokenString->size() &&
      (matchDelimiter('\'', r) &&
       lexer->stringTable.exist(data = tokenString->at(r + 1)) &&
       matchDelimiter('\'', r + 2))) {
    match.stringData.value = data;
    match.stringData.type = MatchData::StringData::string;
    l = r + size;
    return true;
  }

  return false;
}

void Parser::testString() {
  vector<TokenData> tokens;
  int i = 0;

  // null
  tokens = {};
  setTokenString(&tokens);
  assert(matchString(i = 0, i) == false);

  // string
  dataClear();
  tokens = {{4, 9}, lexer->stringTable.put("test"), {4, 9}};
  setTokenString(&tokens);
  assert(matchString(i = 0, i) && i == 3);
  assert(match.stringData.type == MatchData::StringData::string);
  assert(isTokenEqual(match.stringData.value, lexer->stringTable.get("test")));
  lexer->reset();

  // test with offset
  dataClear();
  tokens = {{4, 11}, {4, 9}, lexer->stringTable.put("test"), {4, 9}};
  setTokenString(&tokens);
  assert(matchString(i = 1, i) && i == 3 + 1);
  lexer->reset();

  // size
  dataClear();
  tokens = {{4, 11}, {4, 9}, lexer->stringTable.put("test"), {4, 9}, {4, 9},
            {4, 9},  {4, 9}};
  setTokenString(&tokens);
  assert(matchString(i = 1, i) && i == 3 + 1);
  lexer->reset();

  // integer is syntax error
  dataClear();
  tokens = {{4, 9}, lexer->integerTable.put("1234"), {4, 9}};
  setTokenString(&tokens);
  assert(matchString(i = 0, i) == false);
  lexer->reset();

  // incomplete syntax
  dataClear();
  tokens = {{4, 11}, {4, 9}, lexer->stringTable.put("test")};
  setTokenString(&tokens);
  assert(matchString(i = 0, i) == false);
  lexer->reset();
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

bool Parser::getOPData(int format, int i) {
  if (i >= tokenString->size()) return false;

  TokenData data = tokenString->at(i);
  string mnemonic = lexer->instructionTable.get(data);

  match.opcode = optab->getOPCode(mnemonic);
  match.format = optab->getFormat(mnemonic);

  return match.opcode != -1 && match.format == format;
}

void Parser::testOp() {
  vector<TokenData> tokens;

  // null
  tokens = {};
  setTokenString(&tokens);
  assert(matchOP("ADD", 0) == false);
  assert(matchOP(0) == false);
  assert(getOPData(1, 0) == false);

  // match
  tokens = {{1, 1}};
  setTokenString(&tokens);
  assert(matchOP("ADD", 0));
  assert(matchOP(0));
  assert(getOPData(3, 0));

  // not match
  tokens = {{1, 2}};
  setTokenString(&tokens);
  assert(matchOP("ADD", 0) == false);

  // another table
  tokens = {{2, 1}};
  setTokenString(&tokens);
  assert(matchOP("ADD", 0) == false);
  assert(matchOP(0) == false);
  assert(getOPData(1, 0) == false);

  // format
  tokens = {{1, 2}};
  setTokenString(&tokens);
  assert(getOPData(1, 0) == false);
  assert(getOPData(3, 0) == true);

  tokens = {{1, 3}};
  setTokenString(&tokens);
  assert(getOPData(3, 0) == false);
  assert(getOPData(2, 0) == true);

  // getData
  tokens = {{1, 3}};
  setTokenString(&tokens);
  getOPData(2, 0);
  assert(match.opcode == 0x90);
  assert(match.format == 2);
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
  assert(isTokenEqual(tokens[0], this->match.symbol));

  testReg();
  testN();
  testOp();

  testFmt1();
  testFmt2();
  testFmt3();
  testFmt4();
}
