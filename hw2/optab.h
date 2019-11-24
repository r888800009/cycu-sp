// 資工三甲 林詠翔
// must to use -std=c++11 or higher version

#ifndef _OPTAB_H_
#define _OPTAB_H_

#include <fstream>
#include <unordered_map>

using namespace std;

class OPTab {
  typedef struct Data {
    // string command;
    // int length;
    int opcode, format;
    bool sicxe;
  } Data;

  // <string command, Data>
  unordered_map<string, Data> table;
  fstream fin;

  string toUpper(string upper);

 public:
  OPTab();

  void loadOPTab(const string &);
  int getOPCode(const string &);
  int getFormat(const string &);
  bool isSICXE(const string &);
};

#endif
