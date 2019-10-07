// 資工三甲 林詠翔
// must to use -std=c++11 or higher version
#ifndef _TABLE_H_
#define _TABLE_H_
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

class Table {};

class LoadingTable : public Table {
  string filename;
  fstream fin;
  vector<string> tableData;

 public:
  LoadingTable(const string&);
  void loadtable(const string&);

  int get(const string&);

  enum Error { loading_failure };
};

class HashTable : public Table {
  int hash_fucntion(const string&);

 public:
};

#endif
