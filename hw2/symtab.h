// 資工三甲 林詠翔
// must to use -std=c++11 or higher version

#ifndef _SYMTAB_H_
#define _SYMTAB_H_

#include <unordered_map>

using namespace std;

class SymbolTable {
 public:
  enum Type { absolute_address, relative_address };

 private:
  typedef struct Data {
    int address;
    Type addressType;
  } Data;

  // <string symbol name, Data>
  unordered_map<string, Data> table;

 public:
  SymbolTable();

  void define(const string&, int, Type);
  int getSymbolAddress(const string&);
  Type getSymbolType(const string&);

  enum Error { duplicate_define, undefine_symbol };
};

#endif
