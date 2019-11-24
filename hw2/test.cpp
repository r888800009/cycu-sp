// 資工三甲 林詠翔
// must to use -std=c++11 or higher version
#include "test.h"

#include <cassert>
#include <iostream>
#include <vector>

#include "assembler.h"
#include "error.h"
#include "lexicaler.h"
#include "optab.h"
#include "parser.h"
#include "symtab.h"
#include "table.h"

using namespace std;

void testLoadingTable() {
  LoadingTable instructionTable = LoadingTable(INSTRUCTION_TABLE);
  LoadingTable delimiterTable = LoadingTable(DELIMITER_TABLE);
  instructionTable.loadTable("Table1.table");
  delimiterTable.loadTable("Table4.table");

  // exist
  assert(instructionTable.exist("stt"));
  assert(instructionTable.exist("Stt"));
  assert(!instructionTable.exist("wdddddd"));

  assert(delimiterTable.exist(','));
  assert(!delimiterTable.exist('>'));
  assert(delimiterTable.exist('?'));
  assert(!delimiterTable.exist("asd"));

  // token data
  TokenData data = instructionTable.get("test");
  assert(data.type == -1);
  assert(data.value == -1);

  data = instructionTable.get("Or");
  assert(data.type == 1);
  assert(data.value == 33);

  data = delimiterTable.get('-');
  assert(data.type == 4);
  assert(data.value == 3);

  data = delimiterTable.get('A');
  assert(data.type == -1);
  assert(data.value == -1);

  // token type and value to data
  assert(instructionTable.get((TokenData){-1, -1}) == "");
  assert(instructionTable.get((TokenData){1, 33}) == "OR");
}

void testHashTable() {
  HashTable table1(1);

  // test null
  TokenData data = table1.get("test");
  assert(data.type == -1 && data.value == -1);
  assert(table1.exist({1, ('t' + 'e' + 's' + 't') % 100}) == false);

  // test Upper lower
  table1.put("tesT");
  data = table1.get("test");
  assert(data.type == -1 && data.value == -1);

  data = table1.put("test");
  // cout << data.type << ":" << data.value << endl;
  assert(data.type == 1);
  assert(data.value == ('t' + 'e' + 's' + 't') % 100);
  assert(table1.exist({1, ('t' + 'e' + 's' + 't') % 100}) == true);
  assert(table1.exist({2, ('t' + 'e' + 's' + 't') % 100}) == false);

  data = table1.get("test");
  // cout << data.type << ":" << data.value << endl;
  assert(data.type == 1);
  assert(data.value == ('t' + 'e' + 's' + 't') % 100);

  // collision
  table1.put("ttse");
  data = table1.get("ttse");
  assert(data.type == 1 && data.value == ('t' + 'e' + 's' + 't' + 1) % 100);

  // some value
  data = table1.get("test");
  TokenData data2 = table1.put("test");
  assert(data.value == data2.value);

  // more then size
  table1.reset();
  for (int i = 0; i < HASHTABLE_SIZE; i++) table1.put(to_string(i));

  // full table test
  table1.reset();
  try {
    for (int i = 0; i < HASHTABLE_SIZE + 1; i++) table1.put(to_string(i));
    assert(false);
  } catch (char const *e) {
  }

  // get data
  table1.reset();
  data = table1.put("  t  es t");
  assert(table1.get(data) == "  t  es t");
}

bool equalTokenString(vector<TokenData> tokenStr1,
                      vector<TokenData> tokenStr2) {
  if (tokenStr1.size() != tokenStr2.size()) return false;
}

void testLexer() {
  Lexicaler lexicaler;
  string result;

  vector<string> testcase;

  // test white space and upper lower case
  testcase.push_back("ADD A,92");
  testcase.push_back("ADD        A, 92");
  testcase.push_back(" ADD    A ,92");
  testcase.push_back(" AdD      A , 92   ");

  for (int i = 0; i < testcase.size(); i++)
    assert(lexicaler.lexingLineString(testcase[i]) == "(1,1)(3,1)(4,1)(6,7)");

  // string write space
  lexicaler.reset();
  assert(lexicaler.lexingLineString("MESSAGE DB c'Hello,_World!$  '") ==
         "(5,17)(5,34)(4,9)(7,92)(4,9)");
  assert(lexicaler.getData({7, 92}) == "Hello,_World!$  ");

  lexicaler.reset();
  assert(lexicaler.lexingLineString("MESSAGE DB C'  Hello,_World!$'") ==
         "(5,17)(5,34)(4,9)(7,92)(4,9)");
  assert(lexicaler.getData({7, 92}) == "  Hello,_World!$");

  lexicaler.reset();
  assert(lexicaler.lexingLineString("MESSAGE DB c'  Hello,_World!$  '") ==
         "(5,17)(5,34)(4,9)(7,56)(4,9)");
  assert(lexicaler.getData({7, 56}) == "  Hello,_World!$  ");

  // integer
  lexicaler.reset();
  assert(lexicaler.lexingLineString("MESSAGE DB x'9'") ==
         "(5,17)(5,34)(4,9)(6,57)(4,9)");
  assert(lexicaler.lexingLineString("MESSAGE DB X'9'") ==
         "(5,17)(5,34)(4,9)(6,57)(4,9)");
  assert(lexicaler.getData({6, 57}) == "9");

  // other
  assert(lexicaler.lexingLineString("    ") == "");
  lexicaler.reset();
  assert(lexicaler.lexingLineString("ADD A,9") == "(1,1)(3,1)(4,1)(6,57)");
  lexicaler.reset();
  assert(lexicaler.lexingLineString("   aDd a, 92 . ADD AH, 92") ==
         "(1,1)(3,1)(4,1)(6,7)(4,10)");

  // upper case
  lexicaler.reset();
  assert(lexicaler.lexingLineString("x'ffaA'") ==
         lexicaler.lexingLineString("x'fFaA'"));

  // test getData
  lexicaler.reset();
  lexicaler.lexingLineString("MESSAGE ADD A,9");
  lexicaler.lexingLineString("MESSAGE DB c'  Hello,_World!$  '");
  assert(lexicaler.getData({1, 1}) == "ADD");
  assert(lexicaler.getData({2, 1}) == "START");
  assert(lexicaler.getData({3, 1}) == "A");
  assert(lexicaler.getData({4, 1}) == ",");
  assert(lexicaler.getData({5, 17}) == "MESSAGE");
  assert(lexicaler.getData({6, 57}) == "9");
  assert(lexicaler.getData({7, 56}) == "  Hello,_World!$  ");
}

void testOPTab() {
  OPTab table;

  // load test table
  table.loadOPTab("test-optable.txt");

  // getOPCode
  // ignore case
  assert(table.getOPCode("test1") == 0x10);
  assert(table.getOPCode("TEsT1") == 0x10);

  // check format
  assert(table.getFormat("test1") == 4);
  assert(table.getFormat("Test23") == 3);
  assert(table.getFormat("23test23") == 2);

  // check opcode to hex to dec
  assert(table.getOPCode("test23") == 0xef);
  assert(table.getOPCode("23test23") == 0x2f);

  // null command
  string nullcommand = "tseqywuy";
  assert(table.getOPCode(nullcommand) == -1);
  assert(table.getFormat(nullcommand) == -1);

  // check sic xe
  assert(table.isSICXE("test1") == false);
  assert(table.isSICXE("test23") == true);
  assert(table.isSICXE("23test23") == false);
}

void testSymtab() {
  SymbolTable symtab;

  // duplicate define
  try {
    symtab.define("aaa", 0x00, SymbolTable::absolute_address);
    symtab.define("aaa", 0x00, SymbolTable::absolute_address);
    assert(false);
  } catch (Error::ASMError e) {
    assert(e == Error::duplicate_define);
  }

  // undefine symbol
  try {
    symtab.getSymbolAddress("undefine");
    assert(false);
  } catch (Error::ASMError e) {
    assert(e == Error::undefine_symbol);
  }

  try {
    symtab.getSymbolType("undefine");
    assert(false);
  } catch (Error::ASMError e) {
    assert(e == Error::undefine_symbol);
  }

  // define Symbol
  try {
    symtab.define("relative_address", 0x01, SymbolTable::relative_address);
    assert(symtab.getSymbolType("relative_address") ==
           SymbolTable::relative_address);
    assert(symtab.getSymbolAddress("relative_address") == 0x01);

    symtab.define("absolute_address", 0x02, SymbolTable::absolute_address);
    assert(symtab.getSymbolType("absolute_address") ==
           SymbolTable::absolute_address);
    assert(symtab.getSymbolAddress("absolute_address") == 0x02);
  } catch (Error::ASMError e) {
    assert(false);
  }
}

void testAsmPass1() {
  // duplicate define
}

void testAsmPass2() {
  // undefine
}

void testAsm() {
  Assembler asm1;

  asm1.test();

  testAsmPass1();
  testAsmPass2();
}

void testParser() {
  Lexicaler lexer;
  OPTab optab;

  Parser paser(&lexer, &optab);

  optab.loadOPTab("OPtable.tsv");  // OPTAB
  paser.test();
}

void test() {
  testHashTable();
  testLoadingTable();
  testLexer();
  testOPTab();
  testSymtab();
  testParser();
  testAsm();
}
