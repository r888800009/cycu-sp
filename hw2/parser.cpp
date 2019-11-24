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

bool Parser::matchInteger(const int r, int &l) {
  int hexSize = 3, decSize = 1;
  TokenData data;
  string num;
  try {
    l = r;
    if (r + hexSize - 1 < tokenString->size() &&
        (matchDelimiter('\'', r) &&
         lexer->integerTable.exist(data = tokenString->at(r + 1)) &&
         matchDelimiter('\'', r + 2))) {
      // check format
      num = lexer->integerTable.get(data);
      if (!regex_match(num, regex("[0-9a-fA-F]+"))) return false;

      match.stringData.integer = stoi(num, nullptr, 16);
      match.stringData.value = data;
      match.stringData.type = MatchData::StringData::integer_hex;
      l = r + hexSize;
    } else if (r + decSize - 1 < tokenString->size() &&
               lexer->integerTable.exist(data = tokenString->at(r))) {
      // check format
      num = lexer->integerTable.get(data);
      if (!regex_match(num, regex("[0-9]+"))) return false;

      match.stringData.integer = stoi(lexer->integerTable.get(data));
      match.stringData.value = data;
      match.stringData.type = MatchData::StringData::integer_dec;
      l = r + decSize;
    }

    // check match and range
    return l > r && match.stringData.integer <= 0xffffff;
  } catch (invalid_argument) {
#ifdef DEBUGING
    cout << "do what?" << endl;
    cout << lexer->integerTable.get(data) << endl;
#endif
    return false;
  } catch (out_of_range) {
#ifdef DEBUGING
    cout << "do what?" << endl;
    cout << lexer->integerTable.get(data) << endl;
#endif
    return false;
  }

  return false;
}

void Parser::testInteger() {
  // note: matchInteger would not check integer range
  // only check the size is less then 3 bytes
  vector<TokenData> tokens;
  int i = 0;

  // null
  tokens = {};
  setTokenString(&tokens);
  assert(matchInteger(i = 0, i) == false);

  dataClear();
  tokens = {{4, 9}, lexer->integerTable.put(""), {4, 9}};
  setTokenString(&tokens);
  assert(matchInteger(i = 0, i) == false);
  lexer->reset();

  // hex
  dataClear();
  tokens = {{4, 9}, lexer->integerTable.put("FFFFFF"), {4, 9}};
  setTokenString(&tokens);
  assert(matchInteger(i = 0, i) && i == 3);
  assert(match.stringData.type == MatchData::StringData::integer_hex);
  assert(
      isTokenEqual(match.stringData.value, lexer->integerTable.put("FFFFFF")));
  assert(match.stringData.integer == 0xffffff);
  lexer->reset();

  dataClear();
  tokens = {{4, 9}, lexer->integerTable.put("0"), {4, 9}};
  setTokenString(&tokens);
  assert(matchInteger(i = 0, i) && i == 3);
  assert(match.stringData.integer == 0x0);
  lexer->reset();

  dataClear();
  tokens = {{4, 9}, lexer->integerTable.put("10"), {4, 9}};
  setTokenString(&tokens);
  assert(matchInteger(i = 0, i) && i == 3);
  assert(match.stringData.integer == 0x10);
  lexer->reset();

  // hex case
  dataClear();
  tokens = {{4, 9}, lexer->integerTable.put("abBcCd"), {4, 9}};
  setTokenString(&tokens);
  assert(matchInteger(i = 0, i) && i == 3);
  assert(match.stringData.integer == 0xabbccd);
  lexer->reset();

  // not hex error
  dataClear();
  tokens = {{4, 9}, lexer->integerTable.put("g"), {4, 9}};
  setTokenString(&tokens);
  assert(matchInteger(i = 0, i) == false);
  lexer->reset();

  dataClear();
  tokens = {{4, 9}, lexer->integerTable.put(" ef "), {4, 9}};
  setTokenString(&tokens);
  assert(matchInteger(i = 0, i) == false);
  lexer->reset();

  dataClear();
  tokens = {{4, 9}, lexer->integerTable.put(" 123 123 "), {4, 9}};
  setTokenString(&tokens);
  assert(matchInteger(i = 0, i) == false);
  lexer->reset();

  // dec
  dataClear();
  tokens = {lexer->integerTable.put("16777215")};
  setTokenString(&tokens);
  assert(matchInteger(i = 0, i) && i == 1);
  assert(match.stringData.type == MatchData::StringData::integer_dec);
  assert(isTokenEqual(match.stringData.value,
                      lexer->integerTable.put("16777215")));
  assert(match.stringData.integer == 0xffffff);  /// 0xffffff = 16777215
  lexer->reset();

  dataClear();
  tokens = {lexer->integerTable.put("0")};
  setTokenString(&tokens);
  assert(matchInteger(i = 0, i) && i == 1);
  assert(match.stringData.integer == 0x0);
  lexer->reset();

  dataClear();
  tokens = {lexer->integerTable.put("10")};
  setTokenString(&tokens);
  assert(matchInteger(i = 0, i) && i == 1);
  assert(match.stringData.integer == 10);
  lexer->reset();

  // big num
  tokens = {lexer->integerTable.put("2147483648")};
  setTokenString(&tokens);
  assert(matchInteger(i = 0, i) == false);
  lexer->reset();

  // not dec error
  dataClear();
  tokens = {lexer->integerTable.put("g")};
  setTokenString(&tokens);
  assert(matchInteger(i = 0, i) == false);
  lexer->reset();

  dataClear();
  tokens = {lexer->integerTable.put(" 123 ")};
  setTokenString(&tokens);
  assert(matchInteger(i = 0, i) == false);
  lexer->reset();

  dataClear();
  tokens = {lexer->integerTable.put(" 123 123 ")};
  setTokenString(&tokens);
  assert(matchInteger(i = 0, i) == false);
  lexer->reset();

  dataClear();
  tokens = {lexer->integerTable.put("FFFFFF")};
  setTokenString(&tokens);
  assert(matchInteger(i = 0, i) == false);
  lexer->reset();

  // range error
  dataClear();
  tokens = {{4, 9}, lexer->integerTable.put("1000000"), {4, 9}};
  setTokenString(&tokens);
  assert(matchInteger(i = 0, i) == false);
  lexer->reset();

  dataClear();
  tokens = {lexer->integerTable.put("16777216")};  // 16777216 > 0xffffff
  setTokenString(&tokens);
  assert(matchInteger(i = 0, i) == false);
  lexer->reset();

  // test with offset
  dataClear();
  tokens = {{4, 9}, {4, 9}, lexer->integerTable.put("0"), {4, 9}};
  setTokenString(&tokens);
  assert(matchInteger(i = 1, i) && i == 3 + 1);
  assert(match.stringData.integer == 0x0);
  lexer->reset();

  dataClear();
  tokens = {{4, 9}, lexer->integerTable.put("0")};
  setTokenString(&tokens);
  assert(matchInteger(i = 1, i) && i == 1 + 1);
  assert(match.stringData.integer == 0x0);
  lexer->reset();

  // has next token test size
  dataClear();
  tokens = {{4, 9}, lexer->integerTable.put("0"), {4, 9}, {4, 9}, {4, 9}};
  setTokenString(&tokens);
  assert(matchInteger(i = 0, i) && i == 3);
  assert(match.stringData.integer == 0x0);
  lexer->reset();

  dataClear();
  tokens = {lexer->integerTable.put("0"), {4, 9}, {4, 9}};
  setTokenString(&tokens);
  assert(matchInteger(i = 0, i) && i == 1);
  assert(match.stringData.integer == 0x0);
  lexer->reset();

  // string is syntax error
  dataClear();
  tokens = {{4, 9}, lexer->stringTable.put("1234"), {4, 9}};
  setTokenString(&tokens);
  assert(matchInteger(i = 0, i) == false);
  lexer->reset();

  // incomplete syntax
  dataClear();
  tokens = {{4, 9}, lexer->integerTable.put("0")};
  setTokenString(&tokens);
  assert(matchInteger(i = 0, i) == false);
  lexer->reset();
}

bool Parser::matchString(const int r, int &l) {
  int size = 3;
  TokenData data;
  if (r + size - 1 < tokenString->size() &&
      (matchDelimiter('\'', r) &&
       lexer->stringTable.exist(data = tokenString->at(r + 1)) &&
       matchDelimiter('\'', r + 2))) {
    match.stringData.str = lexer->stringTable.get(data);
    match.stringData.value = data;
    match.stringData.type = MatchData::StringData::string_data;
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
  assert(match.stringData.type == MatchData::StringData::string_data);
  assert(isTokenEqual(match.stringData.value, lexer->stringTable.get("test")));
  assert(match.stringData.str == "test");
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
  testString();
  testInteger();

  testFmt1();
  testFmt2();
  testFmt3();
  testFmt4();
}
