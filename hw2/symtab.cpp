// 資工三甲 林詠翔
// must to use -std=c++11 or higher version

#include "symtab.h"

#include <iostream>

#include "error.h"

using namespace Error;

SymbolTable::SymbolTable() {}

void SymbolTable::define(const string& symbol, int address, Type type) {
  unordered_map<string, Data>::iterator it = table.find(symbol);

  if (it != table.end()) throw duplicate_define;

  table[symbol] = Data{address, type};
}

int SymbolTable::getSymbolAddress(const string& symbol) {
  unordered_map<string, Data>::iterator it = table.find(symbol);

  if (it != table.end()) return it->second.address;

  throw undefine_symbol;
}

SymbolTable::Type SymbolTable::getSymbolType(const string& symbol) {
  unordered_map<string, Data>::iterator it = table.find(symbol);

  if (it != table.end()) return it->second.addressType;

  throw undefine_symbol;
}
