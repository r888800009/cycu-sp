// 資工三甲 林詠翔
// must to use -std=c++11 or higher version

#ifndef _LEXICALER_H_
#define _LEXICALER_H_

#include <vector>
#include "table.h"

using namespace std;

class Lexicaler {
  vector<Table> tables;
  Table &getTable(int index);

 public:
  Lexicaler();
  void loadFile(const string &);
  void writeFile(const string &);
  enum Error { loading_failure };
};

#endif
