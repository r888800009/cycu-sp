// 資工三甲 林詠翔
// must to use -std=c++11 or higher version
#ifndef _TABLE_H_
#define _TABLE_H_
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>

#define INSTRUCTION_TABLE 1
#define PSEUDO_EXTRA_TABLE 2
#define REGISTER_TABLE 3
#define DELIMITER_TABLE 4
#define SYMBOL_TABLE 5
#define INTEGER_REAL_TABLE 6
#define STRING_TABLE 7

#define HASHTABLE_SIZE 100

using namespace std;

typedef struct TokenData {
  int type, value;
} TokenData;

bool isTokenEqual(TokenData t1, TokenData t2);

void printTokenValue(TokenData token);

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
  unordered_map<string, int> tableData;
  unordered_map<int, string> revTableData;

 public:
  LoadingTable(int);
  void loadTable(const string&);
  bool exist(const string& token);
  bool exist(char);
  TokenData get(const string& token);
  TokenData get(char delimiter);
  string get(TokenData);
};

class HashTable : public Table {
  int hash_fucntion(const string&);
  typedef struct HashTableData {
    bool empty = true;
    string value;
  } HashTableData;

  int indexStep(int, int);

  HashTableData tableData[HASHTABLE_SIZE];

 public:
  HashTable(int);

  void reset();
  TokenData put(const string& token);
  TokenData get(const string& token);
  string get(TokenData);
  bool exist(TokenData data);
};

#endif
