// 資工三甲 林詠翔
// must to use -std=c++11 or higher version
#include "test.h"
#include <cassert>
#include <iostream>
#include <vector>
#include "lexicaler.h"
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

  // test Upper lower
  table1.put("tesT");
  data = table1.get("test");
  assert(data.type == -1 && data.value == -1);

  data = table1.put("test");
  // cout << data.type << ":" << data.value << endl;
  assert(data.type == 1);
  assert(data.value == ('t' + 'e' + 's' + 't') % 100);

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
    assert(lexicaler.lexingLine(testcase[i]) == "(1,1)(3,1)(4,1)(6,7)");

  // string write space
  lexicaler.reset();
  assert(lexicaler.lexingLine("MESSAGE DB c'Hello,_World!$  '") ==
         "(5,17)(5,34)(4,9)(7,92)(4,9)");
  assert(lexicaler.getData({7, 92}) == "Hello,_World!$  ");

  lexicaler.reset();
  assert(lexicaler.lexingLine("MESSAGE DB C'  Hello,_World!$'") ==
         "(5,17)(5,34)(4,9)(7,92)(4,9)");
  assert(lexicaler.getData({7, 92}) == "  Hello,_World!$");

  lexicaler.reset();
  assert(lexicaler.lexingLine("MESSAGE DB c'  Hello,_World!$  '") ==
         "(5,17)(5,34)(4,9)(7,56)(4,9)");
  assert(lexicaler.getData({7, 56}) == "  Hello,_World!$  ");

  // other
  assert(lexicaler.lexingLine("    ") == "");
  lexicaler.reset();
  assert(lexicaler.lexingLine("ADD A,9") == "(1,1)(3,1)(4,1)(6,57)");
  lexicaler.reset();
  assert(lexicaler.lexingLine("   aDd a, 92 ; ADD AH, 92") ==
         "(1,1)(3,1)(4,1)(6,7)(4,7)");

  // test getData
  lexicaler.reset();
  lexicaler.lexingLine("MESSAGE ADD A,9");
  lexicaler.lexingLine("MESSAGE DB c'  Hello,_World!$  '");
  assert(lexicaler.getData({1, 1}) == "ADD");
  assert(lexicaler.getData({2, 1}) == "START");
  assert(lexicaler.getData({3, 1}) == "A");
  assert(lexicaler.getData({4, 1}) == ",");
  assert(lexicaler.getData({5, 17}) == "MESSAGE");
  assert(lexicaler.getData({6, 57}) == "9");
  assert(lexicaler.getData({7, 56}) == "  Hello,_World!$  ");
}

void test() {
  testHashTable();
  testLoadingTable();
  testLexer();
}
