// 資工三甲 林詠翔
// must to use -std=c++11 or higher version
#ifndef _TABLE_H_
#define _TABLE_H_
#include <fstream>
#include <iostream>
#include <map>
#include <string>

#define INSTRUCTION_TABLE 1
#define PSEUDO_EXTRA_TABLE 2
#define REGISTER_TABLE 3
#define DELIMITER_TABLE 4
#define SYMBOL_TABLE 5
#define INTEGER_REAL_TABLE 6
#define STRING_TABLE 7

using namespace std;

typedef struct TokenData {
  int type, value;
} TokenData;

class Table {
 protected:
  int tableID;

 public:
  int getTableID();
  virtual TokenData get(const string&) { return {-1, -1}; }
};

class LoadingTable : public Table {
  string filename;
  fstream fin;
  map<string, int> tableData;

 public:
  LoadingTable(int);
  void loadTable(const string&);
  TokenData get(const string& token);

  enum Error { loading_failure };
};

class HashTable : public Table {
  int hash_fucntion(const string&);

 public:
};

#endif