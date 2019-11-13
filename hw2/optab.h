// 資工三甲 林詠翔
// must to use -std=c++11 or higher version

#ifndef _OPTAB_H_
#define _OPTAB_H_

#include <unordered_map>

using namespace std;

class OPTab {
  typedef struct Data {
    string command;
    char opcode;
    int length;
    int format;
  } Data;

  // <string command, Data>
  unordered_map<string, Data> table;

 public:
  OPTab();
  void loadOPTab(const string &);
};

#endif
