// 資工三甲 林詠翔
// must to use -std=c++11 or higher version
#include <cassert>

#include "lexicaler.h"
#include "parser.h"

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

void Parser::testInteger() {
  // note: matchInteger would not check integer range
  // only check the size is less then 3 bytes
  vector<TokenData> tokens;
  int i = 0;

  // null
  tokens = {};
  setTokenString(&tokens);
  assert(matchIntegerHex(i = 0, i) == false);
  assert(matchIntegerDec(i = 0) == false);
  assert(matchIntegerHex(i = 0) == false);

  // hex
  dataClear();
  tokens = {{4, 9}, lexer->integerTable.put(""), {4, 9}};
  setTokenString(&tokens);
  assert(matchIntegerHex(i = 0, i) == false);
  lexer->reset();

  dataClear();
  tokens = {{4, 9}, lexer->integerTable.put("FFFFFF"), {4, 9}};
  setTokenString(&tokens);
  assert(matchIntegerHex(i = 0, i) && i == 3);
  assert(match.stringData.type == MatchData::StringData::integer_hex);
  assert(
      isTokenEqual(match.stringData.value, lexer->integerTable.put("FFFFFF")));
  assert(match.stringData.integer == 0xffffff);
  lexer->reset();

  dataClear();
  tokens = {{4, 9}, lexer->integerTable.put("0"), {4, 9}};
  setTokenString(&tokens);
  assert(matchIntegerHex(i = 0, i) && i == 3);
  assert(match.stringData.integer == 0x0);
  lexer->reset();

  dataClear();
  tokens = {{4, 9}, lexer->integerTable.put("10"), {4, 9}};
  setTokenString(&tokens);
  assert(matchIntegerHex(i = 0, i) && i == 3);
  assert(match.stringData.integer == 0x10);
  lexer->reset();

  // hex case
  dataClear();
  tokens = {{4, 9}, lexer->integerTable.put("abBcCd"), {4, 9}};
  setTokenString(&tokens);
  assert(matchIntegerHex(i = 0, i) && i == 3);
  assert(match.stringData.integer == 0xabbccd);
  lexer->reset();

  // not hex error
  dataClear();
  tokens = {{4, 9}, lexer->integerTable.put("g"), {4, 9}};
  setTokenString(&tokens);
  assert(matchIntegerHex(i = 0, i) == false);
  lexer->reset();

  dataClear();
  tokens = {{4, 9}, lexer->integerTable.put(" ef "), {4, 9}};
  setTokenString(&tokens);
  assert(matchIntegerHex(i = 0, i) == false);
  lexer->reset();

  dataClear();
  tokens = {{4, 9}, lexer->integerTable.put(" 123 123 "), {4, 9}};
  setTokenString(&tokens);
  assert(matchIntegerHex(i = 0, i) == false);
  lexer->reset();

  // dec
  dataClear();
  tokens = {lexer->integerTable.put("16777215")};
  setTokenString(&tokens);
  assert(matchIntegerDec(i = 0));
  assert(match.stringData.type == MatchData::StringData::integer_dec);
  assert(isTokenEqual(match.stringData.value,
                      lexer->integerTable.put("16777215")));
  assert(match.stringData.integer == 0xffffff);  /// 0xffffff = 16777215
  lexer->reset();

  dataClear();
  tokens = {lexer->integerTable.put("0")};
  setTokenString(&tokens);
  assert(matchIntegerDec(i = 0));
  assert(match.stringData.integer == 0x0);
  lexer->reset();

  dataClear();
  tokens = {lexer->integerTable.put("10")};
  setTokenString(&tokens);
  assert(matchIntegerDec(i = 0));
  assert(match.stringData.integer == 10);
  lexer->reset();

  // big num
  tokens = {lexer->integerTable.put("2147483648")};
  setTokenString(&tokens);
  assert(matchIntegerDec(i = 0) == false);
  lexer->reset();

  tokens = lexer->lexingLine("X'100000000'");
  setTokenString(&tokens);
  assert(matchIntegerHex(i = 0, i) == false);
  lexer->reset();

  // not dec error
  dataClear();
  tokens = {lexer->integerTable.put("g")};
  setTokenString(&tokens);
  assert(matchIntegerDec(i = 0) == false);
  lexer->reset();

  dataClear();
  tokens = {lexer->integerTable.put(" 123 ")};
  setTokenString(&tokens);
  assert(matchIntegerDec(i = 0) == false);
  lexer->reset();

  dataClear();
  tokens = {lexer->integerTable.put(" 123 123 ")};
  setTokenString(&tokens);
  assert(matchIntegerDec(i = 0) == false);
  lexer->reset();

  dataClear();
  tokens = {lexer->integerTable.put("FFFFFF")};
  setTokenString(&tokens);
  assert(matchIntegerDec(i = 0) == false);
  lexer->reset();

  // range error
  dataClear();
  tokens = {{4, 9}, lexer->integerTable.put("1000000"), {4, 9}};
  setTokenString(&tokens);
  assert(matchIntegerHex(i = 0, i) == false);
  lexer->reset();

  dataClear();
  tokens = {lexer->integerTable.put("16777216")};  // 16777216 > 0xffffff
  setTokenString(&tokens);
  assert(matchIntegerDec(i = 0) == false);
  lexer->reset();

  // test with offset
  dataClear();
  tokens = {{4, 9}, {4, 9}, lexer->integerTable.put("0"), {4, 9}};
  setTokenString(&tokens);
  assert(matchIntegerHex(i = 1, i) && i == 3 + 1);
  assert(match.stringData.integer == 0x0);
  lexer->reset();

  dataClear();
  tokens = {{4, 9}, lexer->integerTable.put("0")};
  setTokenString(&tokens);
  assert(matchIntegerDec(i = 1));
  assert(match.stringData.integer == 0x0);
  lexer->reset();

  // has next token test size
  dataClear();
  tokens = {{4, 9}, lexer->integerTable.put("0"), {4, 9}, {4, 9}, {4, 9}};
  setTokenString(&tokens);
  assert(matchIntegerHex(i = 0, i) && i == 3);
  assert(match.stringData.integer == 0x0);
  lexer->reset();

  dataClear();
  tokens = {lexer->integerTable.put("0"), {4, 9}, {4, 9}};
  setTokenString(&tokens);
  assert(matchIntegerDec(i = 0));
  assert(match.stringData.integer == 0x0);
  lexer->reset();

  // string is syntax error
  dataClear();
  tokens = {{4, 9}, lexer->stringTable.put("1234"), {4, 9}};
  setTokenString(&tokens);
  assert(matchIntegerHex(i = 0, i) == false);
  lexer->reset();

  // incomplete syntax
  dataClear();
  tokens = {{4, 9}, lexer->integerTable.put("0")};
  setTokenString(&tokens);
  assert(matchIntegerHex(i = 0, i) == false);
  lexer->reset();

  dataClear();
  tokens = lexer->lexingLine("'123'");
  setTokenString(&tokens);
  assert(matchIntegerHex(i = 0, i) == false);
  lexer->reset();

  // hex without delimiter
  dataClear();
  tokens = lexer->lexingLine("0");
  setTokenString(&tokens);
  assert(matchIntegerHex(0));
  assert(match.stringData.type == MatchData::StringData::integer_hex);
  assert(match.stringData.integer == 0x0);
  lexer->reset();

  dataClear();
  tokens = lexer->lexingLine("ffff");
  setTokenString(&tokens);
  assert(matchIntegerHex(0));
  assert(match.stringData.type == MatchData::StringData::integer_hex);
  assert(match.stringData.integer == 0xffff);
  lexer->reset();

  dataClear();
  tokens = lexer->lexingLine("fffff");
  setTokenString(&tokens);
  assert(matchIntegerHex(0) != true);
  lexer->reset();

  dataClear();
  tokens = lexer->lexingLine(" g g ");
  setTokenString(&tokens);
  assert(matchIntegerHex(0) != true);
  lexer->reset();
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

  dataClear();
  tokens = lexer->lexingLine("'123'");
  setTokenString(&tokens);
  assert(matchString(i = 0, i) == false);
  lexer->reset();
}

void Parser::testOp() {
  vector<TokenData> tokens;

  setXE(false);
  // sic
  tokens = {{1, 1}};
  setTokenString(&tokens);
  assert(getOPData(3, 0));

  // not sic
  tokens = lexer->lexingLine("ldb");
  setTokenString(&tokens);
  assert(getOPData(3, 0) == false);

  setXE(true);
  tokens = lexer->lexingLine("ldb");
  setTokenString(&tokens);
  assert(getOPData(3, 0) == true);

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

  // big num
  tokens = {lexer->integerTable.put("2147483647")};
  setTokenString(&tokens);
  assert(matchN(0) == -1);
  lexer->reset();

  tokens = {lexer->integerTable.put("2147483648")};
  setTokenString(&tokens);
  assert(matchN(0) == -1);
  lexer->reset();

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

void Parser::testMemory() {
  vector<TokenData> tokens;

  // null
  tokens = {};
  setTokenString(&tokens);
  assert(matchMemory(0) == false);

  // symbol
  lexer->lexingLine("symbol1 EQU 123");  // add symbol
  tokens = lexer->lexingLine("symbol1");
  setTokenString(&tokens);
  assert(matchMemory(0));
  assert(isTokenEqual(match.memory, lexer->symbolTable.get("symbol1")));
  lexer->reset();

  // integer
  tokens = lexer->lexingLine("1234");
  setTokenString(&tokens);
  assert(matchMemory(0));
  assert(isTokenEqual(match.memory, lexer->integerTable.get("1234")));
  assert(match.stringData.integer == 1234);
  lexer->reset();

  // size
  tokens = lexer->lexingLine("symbol1 EQU 123 ,,,,");
  setTokenString(&tokens);
  assert(matchMemory(2));

  tokens = lexer->lexingLine("symbol2 EQU symbol ,,,,");
  setTokenString(&tokens);
  assert(matchMemory(2));
  lexer->reset();

  // dont match hex
  tokens = lexer->lexingLine("X'123'");
  setTokenString(&tokens);
  assert(matchMemory(0) == false);
  lexer->reset();
}

void Parser::testReg() {
  vector<TokenData> tokens;

  // null
  tokens = {};
  setTokenString(&tokens);
  assert(matchRegister(0) == -1);
  assert(matchRegister("X", 0) == false);

  // has reg
  for (int i = 1; i <= 9; i++) {
    tokens = {{3, i}};
    setTokenString(&tokens);
    assert(matchRegister(0) == i - 1);
    assert(matchRegister(lexer->registerTable.get(tokens[0]), 0));
  }

  // not reg
  tokens = {{1, 1}};
  setTokenString(&tokens);
  assert(matchRegister(0) == -1);
  assert(matchRegister(lexer->registerTable.get(TokenData{3, 1}), 0) == false);

  tokens = {{2, 1}};
  setTokenString(&tokens);
  assert(matchRegister(0) == -1);

  // match reg
  tokens = {{3, 4}};
  setTokenString(&tokens);
  assert(matchRegister(lexer->registerTable.get(TokenData{3, 4}), 0));
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
  assert(isTokenEqual(tokens[0], this->tmpSymbol));

  testOp();
  testReg();
  testN();
  testString();
  testInteger();

  testMemory();

  testFmt1();
  testFmt2();
  testFmt3();
  testFmt4();

  testMode();
  testPseudo();
}

void Parser::testSTART() {
  vector<TokenData> tokens;
  int i;

  tokens = lexer->lexingLine("START 1");
  setTokenString(&tokens);
  assert(matchPseudo(i = 0, i) && i == 2);
  assert(match.pseudo == START);
  assert(match.startMatch == 1);
  lexer->reset();

  tokens = lexer->lexingLine("START 1000");
  setTokenString(&tokens);
  assert(matchPseudo(i = 0, i) && i == 2);
  assert(match.startMatch == 0x1000);
  lexer->reset();

  tokens = lexer->lexingLine("START symbol");
  setTokenString(&tokens);
  assert(matchPseudo(i = 0, i) == false);
  lexer->reset();

  tokens = lexer->lexingLine("symbol START 1");
  setTokenString(&tokens);
  assert(matchPseudo(i = 0, i) && i == 3);
  assert(match.startMatch == 1);
  lexer->reset();

  tokens = lexer->lexingLine("symbol START x'1'");
  setTokenString(&tokens);
  assert(matchPseudo(i = 0, i) == false);
  lexer->reset();

  tokens = lexer->lexingLine("START");
  setTokenString(&tokens);
  assert(matchPseudo(i = 0, i) == false);
  lexer->reset();
}

void Parser::testEQU() {
  vector<TokenData> tokens;
  int i;

  tokens = lexer->lexingLine("SYMBOL1 EQU SSSSymbol");
  setTokenString(&tokens);
  assert(matchPseudo(i = 0, i) && i == 3);
  assert(lexer->symbolTable.get(match.preSymbol) == "SYMBOL1");
  assert(lexer->symbolTable.get(match.equMatch[0]) == "SSSSymbol");
  assert(match.equMatch.size() == 1);
  assert(match.pseudo == EQU);
  lexer->reset();

  tokens = lexer->lexingLine("EQU SSSSymbol");
  setTokenString(&tokens);
  assert(matchPseudo(i = 0, i) == false);
  lexer->reset();

  tokens = lexer->lexingLine("EQU");
  setTokenString(&tokens);
  assert(matchPseudo(i = 0, i) == false);
  lexer->reset();

  tokens = lexer->lexingLine("symb EQU *");
  setTokenString(&tokens);
  assert(matchPseudo(i = 0, i) && i == 3);
  assert(match.equMatch.size() == 1);
  lexer->reset();

  tokens = lexer->lexingLine("symb EQU 123");
  setTokenString(&tokens);
  assert(matchPseudo(i = 0, i) && i == 3);
  assert(match.equMatch.size() == 1);
  lexer->reset();
}

void Parser::testEQUAdv() {
  vector<TokenData> tokens;
  int i;
  tokens = lexer->lexingLine("symb EQU abc+1");
  setTokenString(&tokens);
  assert(matchPseudo(i = 0, i) == false);
  lexer->reset();

  tokens = lexer->lexingLine("symb EQU abc+");
  setTokenString(&tokens);
  assert(matchPseudo(i = 0, i) == false);
  lexer->reset();

  // register
  tokens = lexer->lexingLine("symb EQU abc+a");
  setTokenString(&tokens);
  assert(matchPseudo(i = 0, i) == false);
  lexer->reset();

  tokens = lexer->lexingLine("symb EQU +");
  setTokenString(&tokens);
  assert(matchPseudo(i = 0, i) == false);
  lexer->reset();

  tokens = lexer->lexingLine("symb EQU ++");
  setTokenString(&tokens);
  assert(matchPseudo(i = 0, i) == false);
  lexer->reset();

  tokens = lexer->lexingLine("symb EQU abc+abc+");
  setTokenString(&tokens);
  assert(matchPseudo(i = 0, i) == false);
  lexer->reset();

  tokens = lexer->lexingLine("symb EQU vvv-avvv");
  setTokenString(&tokens);
  assert(matchPseudo(i = 0, i) && i == 5);
  assert(match.equMatch.size() == 3);
  lexer->reset();

  tokens = lexer->lexingLine("symb EQU vvv*avvv");
  setTokenString(&tokens);
  assert(matchPseudo(i = 0, i) && i == 5);
  assert(match.equMatch.size() == 3);
  lexer->reset();

  tokens = lexer->lexingLine("symb EQU vvv/avvv");
  setTokenString(&tokens);
  assert(matchPseudo(i = 0, i) && i == 5);
  assert(match.equMatch.size() == 3);
  lexer->reset();

  tokens = lexer->lexingLine("symb EQU vvv+avvv");
  setTokenString(&tokens);
  assert(matchPseudo(i = 0, i) && i == 5);
  assert(match.equMatch.size() == 3);
  lexer->reset();

  tokens = lexer->lexingLine("symb EQU vvv+avvv");
  setTokenString(&tokens);
  assert(matchPseudo(i = 0, i) && i == 5);
  assert(match.equMatch.size() == 3);
  lexer->reset();

  /*
    def get1(str):
      i = 0
      for c in str:
        i += ord(c)
      print(i%100)
      +: (4,2)
      -: (4,3)
      *: (4,4)
      /: (4,5)
      symbol table: 5
      aaaa: 88
      bbbb: 92
      cccc: 96
      dddd: 0
   */

  // postorder
  // aaaa bbbb cccc * dddd / +
  vector<TokenData> postorder = {{5, 88}, {5, 92}, {5, 96}, {4, 4},
                                 {5, 0},  {4, 5},  {4, 2}};
  tokens = lexer->lexingLine("symb EQU aaaa+bbbb*cccc/dddd");
  setTokenString(&tokens);
  assert(matchPseudo(i = 0, i) && i == 9);
  assert(match.equMatch.size() == 7);
  assert(equal(postorder.begin(), postorder.end(), match.equMatch.begin(),
               isTokenEqual));
  lexer->reset();

  // aaaa bbbb + cccc + dddd +
  postorder = {{5, 88}, {5, 92}, {4, 2}, {5, 96}, {4, 2}, {5, 0}, {4, 2}};
  tokens = lexer->lexingLine("symb EQU aaaa+bbbb+cccc+dddd");
  setTokenString(&tokens);
  assert(matchPseudo(i = 0, i) && i == 9);
  assert(match.equMatch.size() == 7);
  assert(equal(postorder.begin(), postorder.end(), match.equMatch.begin(),
               isTokenEqual));
  lexer->reset();
}

void Parser::testEND() {
  vector<TokenData> tokens;
  int i;

  tokens = lexer->lexingLine("END FIRST");
  setTokenString(&tokens);
  assert(matchPseudo(i = 0, i) == true && i == 2);
  assert(match.pseudo == END);
  lexer->reset();

  tokens = lexer->lexingLine("symbol END FIRST");
  setTokenString(&tokens);
  assert(matchPseudo(i = 0, i) == true && i == 3);
  assert(lexer->symbolTable.get(match.preSymbol) == "symbol");
  assert(lexer->symbolTable.get(match.memory) == "FIRST");
  lexer->reset();

  tokens = lexer->lexingLine("END");
  setTokenString(&tokens);
  assert(matchPseudo(i = 0, i) == true && i == 1);
  lexer->reset();

  // Do not test get symbol for now
}

void Parser::testBYTE() {
  vector<TokenData> tokens;
  int i;

  tokens = lexer->lexingLine("BYTE c'123'");
  setTokenString(&tokens);
  assert(matchPseudo(i = 0, i) && i == 4);
  assert(match.pseudo == BYTE);
  assert(match.stringData.type == MatchData::StringData::string_data);
  lexer->reset();

  tokens = lexer->lexingLine("Symbol BYTE c'123'");
  setTokenString(&tokens);
  assert(matchPseudo(i = 0, i) && i == 5);
  assert(match.stringData.type == MatchData::StringData::string_data);
  lexer->reset();

  tokens = lexer->lexingLine("BYTE c''");
  setTokenString(&tokens);
  assert(matchPseudo(i = 0, i) && i == 4);
  assert(match.stringData.type == MatchData::StringData::string_data);
  lexer->reset();

  tokens = lexer->lexingLine("BYTE x'1'");
  setTokenString(&tokens);
  assert(matchPseudo(i = 0, i) && i == 4);
  assert(match.stringData.type == MatchData::StringData::integer_hex);
  lexer->reset();

  tokens = lexer->lexingLine("BYTE x''");
  setTokenString(&tokens);
  assert(matchPseudo(i = 0, i) == false);
  lexer->reset();

  tokens = lexer->lexingLine("BYTE x'fff'");
  setTokenString(&tokens);
  assert(matchPseudo(i = 0, i) == false);
  lexer->reset();

  tokens = lexer->lexingLine("BYTE x'ff'");
  setTokenString(&tokens);
  assert(matchPseudo(i = 0, i) && i == 4);
  assert(match.stringData.type == MatchData::StringData::integer_hex);
  lexer->reset();
}

void Parser::testWORD() {
  vector<TokenData> tokens;
  int i;

  tokens = lexer->lexingLine("WORD c'123'");
  setTokenString(&tokens);
  assert(matchPseudo(i = 0, i) && i == 4);
  assert(match.pseudo == WORD);
  assert(match.stringData.type == MatchData::StringData::string_data);
  lexer->reset();

  tokens = lexer->lexingLine("Symbol WORD c'123'");
  setTokenString(&tokens);
  assert(matchPseudo(i = 0, i) && i == 5);
  lexer->reset();

  tokens = lexer->lexingLine("WORD c''");
  setTokenString(&tokens);
  assert(matchPseudo(i = 0, i) && i == 4);
  assert(match.stringData.type == MatchData::StringData::string_data);
  lexer->reset();

  tokens = lexer->lexingLine("WORD x'1'");
  setTokenString(&tokens);
  assert(matchPseudo(i = 0, i) && i == 4);
  assert(match.stringData.type == MatchData::StringData::integer_hex);
  lexer->reset();

  tokens = lexer->lexingLine("WORD x''");
  setTokenString(&tokens);
  assert(matchPseudo(i = 0, i) == false);
  lexer->reset();

  tokens = lexer->lexingLine("WORD x'fffffff'");
  setTokenString(&tokens);
  assert(matchPseudo(i = 0, i) == false);
  lexer->reset();

  tokens = lexer->lexingLine("WORD x'ffffff'");
  setTokenString(&tokens);
  assert(matchPseudo(i = 0, i) && i == 4);
  assert(match.stringData.type == MatchData::StringData::integer_hex);
  lexer->reset();

  tokens = lexer->lexingLine("WORD 213");
  setTokenString(&tokens);
  assert(matchPseudo(i = 0, i) && i == 2);
  cout << i << endl;
  assert(match.stringData.type == MatchData::StringData::integer_dec);
  lexer->reset();
}

void Parser::testRESB() {
  vector<TokenData> tokens;
  int i;

  tokens = lexer->lexingLine("RESB 1");
  setTokenString(&tokens);
  assert(matchPseudo(i = 0, i) && i == 2);
  assert(match.pseudo == RESB);
  assert(lexer->integerTable.get(match.resMatch) == "1");
  lexer->reset();

  tokens = lexer->lexingLine("RESB symbol");
  setTokenString(&tokens);
  assert(matchPseudo(i = 0, i) == false);
  lexer->reset();

  tokens = lexer->lexingLine("symbol RESB 1");
  setTokenString(&tokens);
  assert(matchPseudo(i = 0, i) && i == 3);
  lexer->reset();
}

void Parser::testRESW() {
  vector<TokenData> tokens;
  int i;

  tokens = lexer->lexingLine("RESW 1");
  setTokenString(&tokens);
  assert(matchPseudo(i = 0, i) && i == 2);
  assert(match.pseudo == RESW);
  assert(lexer->integerTable.get(match.resMatch) == "1");
  assert(match.stringData.integer == 1);
  lexer->reset();

  tokens = lexer->lexingLine("RESW symbol");
  setTokenString(&tokens);
  assert(matchPseudo(i = 0, i) == false);
  lexer->reset();

  tokens = lexer->lexingLine("symbol RESW 1");
  setTokenString(&tokens);
  assert(matchPseudo(i = 0, i) && i == 3);
  lexer->reset();
}

void Parser::testBASE() {
  vector<TokenData> tokens;
  int i;

  tokens = lexer->lexingLine("BASE LENGTH");
  setTokenString(&tokens);
  assert(matchPseudo(i = 0, i) && i == 2);
  assert(match.pseudo == BASE);
  assert(lexer->symbolTable.get(match.baseMatch) == "LENGTH");
  lexer->reset();

  tokens = lexer->lexingLine("BASE 123");
  setTokenString(&tokens);
  assert(matchPseudo(i = 0, i) && i == 2);
  assert(lexer->integerTable.get(match.baseMatch) == "123");
  lexer->reset();

  tokens = lexer->lexingLine("symbol BASE 123");
  setTokenString(&tokens);
  assert(matchPseudo(i = 0, i) && i == 3);
  lexer->reset();
}

void Parser::testLTORG() {
  vector<TokenData> tokens;
  int i;

  tokens = lexer->lexingLine("LTORG");
  setTokenString(&tokens);
  assert(matchPseudo(i = 0, i) && i == 1);
  assert(match.pseudo == LTORG);
  lexer->reset();

  tokens = lexer->lexingLine("symbol LTORG");
  setTokenString(&tokens);
  assert(matchPseudo(i = 0, i) && i == 2);
  lexer->reset();
}

void Parser::testPseudo() {
  vector<TokenData> tokens;
  int i;

  // null token test
  tokens = {};
  setTokenString(&tokens);
  assert(matchPseudo(i = 0, i) == false);
  lexer->reset();

  testSTART();
  testEND();
  testBYTE();
  testWORD();
  testRESB();
  testRESW();
  testBASE();
  testLTORG();
  testEQU();

  testEQUAdv();
}

void Parser::testMode() {
  cout << "SIC no test!!" << endl;
  // matchSyntax handle

  // sic mode
  // fmt1 disable
  // fmt2 disable
  // fmt3 enable
  // fmt4 disable

  // sicxe mode
  // fmt1 enable
  // fmt2 enable
  // fmt3 enable
  // fmt4 enable
}

void Parser::testFmt3() {
  vector<TokenData> tokens;
  int i = 0;

  // null
  tokens = {};
  setTokenString(&tokens);
  assert(matchFormat3(i = 0, i) == false);
  lexer->reset();

  // size more then
  tokens = lexer->lexingLine("LDA 1,,");
  setTokenString(&tokens);
  assert(matchFormat3(i = 0, i) && i == 2);
  lexer->reset();

  // sic mode make should simple addressing
  setXE(false);
  // simple addressing true
  tokens = lexer->lexingLine("LDA 1");
  setTokenString(&tokens);
  assert(matchFormat3(i = 0, i) && i == 2);
  assert(match.addrType == AddressingType::simple_addressing);
  assert(match.x == false);
  assert(match.opcode == 0x00);
  assert(match.format == 3);
  lexer->reset();

  // indirect false
  tokens = lexer->lexingLine("LDA @1");
  setTokenString(&tokens);
  assert(matchFormat3(i = 0, i) == false);
  lexer->reset();

  // immediate false
  tokens = lexer->lexingLine("LDA #1");
  setTokenString(&tokens);
  assert(matchFormat3(i = 0, i) == false);
  lexer->reset();

  // sic not support mnemonic
  tokens = lexer->lexingLine("LDB 1");
  setTokenString(&tokens);
  assert(matchFormat3(i = 0, i) == false);
  lexer->reset();

  // sicxe mode make should simple addressing
  setXE(true);
  // simple addressing true
  tokens = lexer->lexingLine("LDA 1");
  setTokenString(&tokens);
  assert(matchFormat3(i = 0, i) && i == 2);
  assert(match.addrType == AddressingType::simple_addressing);
  lexer->reset();

  // indirect true
  tokens = lexer->lexingLine("LDA @1");
  setTokenString(&tokens);
  assert(matchFormat3(i = 0, i) && i == 3);
  assert(match.addrType == AddressingType::indirect_addressing);
  lexer->reset();

  // immediate true
  tokens = lexer->lexingLine("LDA #1");
  setTokenString(&tokens);
  assert(matchFormat3(i = 0, i) && i == 3);
  assert(match.addrType == AddressingType::immediate_addressing);
  lexer->reset();

  // test X reg
  tokens = lexer->lexingLine("LDA #1,X");
  setTokenString(&tokens);
  // fmt3 does not check is string end.
  assert(matchFormat3(i = 0, i) == true && i == 3);
  lexer->reset();

  tokens = lexer->lexingLine("LDA 1,X");
  setTokenString(&tokens);
  assert(matchFormat3(i = 0, i) && i == 4);
  assert(match.x == true);
  assert(match.addrType == AddressingType::simple_addressing);
  lexer->reset();

  // with size
  tokens = lexer->lexingLine("LDA 1,X,,,");
  setTokenString(&tokens);
  assert(matchFormat3(i = 0, i) && i == 4);
  assert(match.x == true);
  lexer->reset();

  // literals
  tokens = lexer->lexingLine("LDA =x'1'");
  setTokenString(&tokens);
  assert(matchFormat3(i = 0, i) && i == 5);
  assert(isTokenEqual(match.literal, lexer->integerTable.get("1")));
  assert(match.addrType == AddressingType::simple_addressing);
  assert(match.x == false);
  lexer->reset();

  tokens = lexer->lexingLine("LDA =c' string '");
  setTokenString(&tokens);
  assert(matchFormat3(i = 0, i) && i == 5);
  assert(isTokenEqual(match.literal, lexer->stringTable.get(" string ")));
  assert(match.addrType == AddressingType::simple_addressing);
  assert(match.x == false);
  lexer->reset();

  // fmt3 does not check is string end.
  tokens = lexer->lexingLine("LDA =x'1',X");
  setTokenString(&tokens);
  assert(matchFormat3(i = 0, i) && i == 5);
  lexer->reset();

  // rsub
  tokens = lexer->lexingLine("RSUB");
  setTokenString(&tokens);
  assert(matchFormat3(i = 0, i) && i == 1);
  assert(match.addrType == AddressingType::simple_addressing);
  lexer->reset();
}

void Parser::testFmt4() {
  vector<TokenData> tokens;
  int i = 0;
  // work on sicxe mode, sic mode would error

  // null
  tokens = {};
  setTokenString(&tokens);
  assert(matchFormat4(i = 0, i) == false);
  lexer->reset();

  // size more then
  tokens = lexer->lexingLine("+LDA 1,,");
  setTokenString(&tokens);
  assert(matchFormat4(i = 0, i) && i == 3);
  lexer->reset();

  setXE(true);
  // test fmt4
  // sicxe mode make should simple addressing
  // simple addressing
  tokens = lexer->lexingLine("+LDA 1");
  setTokenString(&tokens);
  assert(matchFormat4(i = 0, i) && i == 3);
  assert(match.format == 4);
  lexer->reset();

  // indirect true
  tokens = lexer->lexingLine("+LDA @1");
  setTokenString(&tokens);
  assert(matchFormat4(i = 0, i) && i == 4);
  lexer->reset();

  // immediate true
  tokens = lexer->lexingLine("+LDA #1");
  setTokenString(&tokens);
  assert(matchFormat4(i = 0, i) && i == 4);
  lexer->reset();

  // X reg
  tokens = lexer->lexingLine("+LDA 1,X");
  setTokenString(&tokens);
  assert(matchFormat4(i = 0, i) && i == 5);
  lexer->reset();

  // test fmt3 would not match
  // simple addressing
  tokens = lexer->lexingLine("LDA 1");
  setTokenString(&tokens);
  assert(matchFormat4(i = 0, i) == false);
  lexer->reset();

  // indirect true
  tokens = lexer->lexingLine("LDA @1");
  setTokenString(&tokens);
  assert(matchFormat4(i = 0, i) == false);
  lexer->reset();

  // immediate true
  tokens = lexer->lexingLine("LDA #1");
  setTokenString(&tokens);
  assert(matchFormat4(i = 0, i) == false);
  lexer->reset();

  // X reg
  tokens = lexer->lexingLine("LDA 1,X");
  setTokenString(&tokens);
  assert(matchFormat4(i = 0, i) == false);
  lexer->reset();
}

