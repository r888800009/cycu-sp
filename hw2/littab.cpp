// 資工三甲 林詠翔
// must to use -std=c++11 or higher version

#include "littab.h"

#include <algorithm>
#include <cassert>

#include "error.h"

int test_hash(const string& str) {
  int result = 0;

  for (int i = 0; i < str.length(); i++)
    result = (result + str.at(i)) % HASHTABLE_SIZE;

  return result;
}

LiteralTable::LiteralTable() : tableData1(10) { reset(); }

void LiteralTable::reset() {
  tableData1.reset();
  for (int i = 0; i < HASHTABLE_SIZE; i++) {
    tableData2[i].address = -1;
    tableData2[i].gen = false;
    tableData2[i].empty = true;
    pass2Wait[i] = false;
  }
}

void LiteralTable::test() {
  string str;
  int hash;
  const int intTab = INTEGER_REAL_TABLE, strTab = STRING_TABLE;

  // test put
  str = "0";
  hash = test_hash(str);
  put(str, {intTab, 1});
  assert(tableData2[hash].address == -1);
  assert(tableData2[hash].empty == false);
  assert(tableData2[hash].gen == false);
  assert(pass2Wait[hash] == false);
  assert(isTokenEqual(tableData2[hash].token, {intTab, 1}));

  // test get not gen
  assert(getAddress({intTab, 1}) == -1);

  // test genLiteral
  put("1", {intTab, 2});
  put("00", {strTab, 3});
  put("2", {intTab, 4});
  put("3", {intTab, 5});

  int pc = 0;
  vector<TokenData> genTokens1;
  pc = genLiteral(pc, genTokens1);
  assert(pc == 6);
  assert(genTokens1.size() == 5);

  // test get gened
  assert(getAddress("0") == 0);
  assert(getAddress("1") == 1);
  assert(getAddress("2") == 2);
  assert(getAddress("3") == 3);
  assert(getAddress("00") == 4);

  // test again
  put("4", {intTab, 6});
  put("5", {intTab, 7});

  vector<TokenData> genTokens2;
  pc = genLiteral(pc, genTokens2);
  assert(pc == 8);

  // pass2
  pc = 0;
  putPass2("0");
  putPass2("1");
  putPass2("00");
  putPass2("2");
  putPass2("3");
  vector<TokenData> genTokens1to2;
  pc = genLiteralPass2(pc, genTokens1to2);
  assert(pc == 6);
  assert(genTokens1to2.size() == 5);

  // test again
  putPass2("4");
  putPass2("5");

  vector<TokenData> genTokens2to2;
  pc = genLiteralPass2(pc, genTokens2to2);
  assert(pc == 8);

  assert(equal(genTokens1.begin(), genTokens1.end(), genTokens1to2.begin(),
               isTokenEqual));

  assert(equal(genTokens2.begin(), genTokens2.end(), genTokens2to2.begin(),
               isTokenEqual));
}

TokenData LiteralTable::put(const string& value, TokenData token) {
  int hash_value = tableData1.get(value).value;
  if (hash_value != -1 && !tableData2[hash_value].empty) return {-1, -1};

  hash_value = tableData1.put(value).value;
  tableData2[hash_value].empty = false;
  tableData2[hash_value].gen = false;
  tableData2[hash_value].address = -1;
  tableData2[hash_value].token = token;
  tableData2[hash_value].value = value;
}

int LiteralTable::getAddress(const string& value) {
  int hash_value = tableData1.get(value).value;
  if (hash_value != -1 && tableData2[hash_value].empty ||
      !tableData2[hash_value].gen)
    return -1;

  return tableData2[hash_value].address;
}

void LiteralTable::putPass2(const string& value) {
  int hash_value = tableData1.get(value).value;
  if (hash_value != -1 && tableData2[hash_value].empty ||
      !tableData2[hash_value].gen)
    return;
  pass2Wait[hash_value] = true;
}

int LiteralTable::genLiteralPass2(int curPC, vector<TokenData>& genTokens) {
  for (int i = 0; i < HASHTABLE_SIZE; i++) {
    if (pass2Wait[i]) {
      TokenData token = tableData2[i].token;
      if (token.type == STRING_TABLE) curPC += tableData2[i].value.size();
      if (token.type == INTEGER_REAL_TABLE) {
        int value = stoi(tableData2[i].value, nullptr, 16);
        // 1 bytes
        if (value <= 0xff)
          curPC += 1;
        else
          curPC += 3;
      }

      genTokens.push_back(token);
      pass2Wait[i] = false;
    }
  }

  return curPC;
}

// return new pc
int LiteralTable::genLiteral(int curPC, vector<TokenData>& genTokens) {
  for (int i = 0; i < HASHTABLE_SIZE; i++) {
    if (!tableData2[i].empty && tableData2[i].gen == false) {
      tableData2[i].gen = true;
      tableData2[i].address = curPC;
      TokenData token = tableData2[i].token;
      if (token.type == STRING_TABLE) curPC += tableData2[i].value.size();
      if (token.type == INTEGER_REAL_TABLE) {
        int value = stoi(tableData2[i].value, nullptr, 16);
        // 1 bytes
        if (value <= 0xff)
          curPC += 1;
        else
          curPC += 3;
      }

      genTokens.push_back(token);
    }
  }

  return curPC;
}

