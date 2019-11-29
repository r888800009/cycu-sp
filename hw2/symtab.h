// 資工三甲 林詠翔
// must to use -std=c++11 or higher version

#ifndef _SYMTAB_H_
#define _SYMTAB_H_

using namespace std;

class SymbolTable {
 public:
  enum Type { absolute_address, symbol_reference, null_symbol };

 private:
  typedef struct Data {
    int value = -1;
    Type addressType = null_symbol;
  } Data;

  // <string symbol name, Data>
  Data table[100] = {0};

 public:
  SymbolTable();

  void define(int value, int, Type);
  int getSymbolAddress(int value);
  void checkUndefine();
  Type getSymbolType(int value);
};

#endif
