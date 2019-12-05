// 資工三甲 林詠翔
// must to use -std=c++11 or higher version

#ifndef _LITTAB_H_
#define _LITTAB_H_

#include <vector>

#include "table.h"

class LiteralTable {
  typedef struct HashTableData {
    int address;
    TokenData token;
    bool gen, empty;
    string value;
  } HashTableData;

  HashTable tableData1;
  HashTableData tableData2[HASHTABLE_SIZE];
  bool pass2Wait[HASHTABLE_SIZE];

 public:
  LiteralTable();
  void reset();
  TokenData put(const string& value, TokenData token);

  int getAddress(const string& value);

  // return new pc
  int genLiteral(int curPC, vector<TokenData>& genTokens);

  void putPass2(const string& value);
  int genLiteralPass2(int curPC, vector<TokenData>& genTokens);

  void test();
};
#endif
