// 資工三甲 林詠翔
// must to use -std=c++11 or higher version

#include "symtab.h"

#include <algorithm>
#include <iostream>

#include "error.h"

using namespace Error;

SymbolTable::SymbolTable() { fill_n(table, 100, Data{-1, null_symbol}); }

void checkUndefine();

void SymbolTable::define(int value, int address, Type type) {
  if (table[value].value != -1) throw duplicate_define;

  table[value] = {address, type};
}

int SymbolTable::getSymbolAddress(int value) {
  if (table[value].value != -1) return table[value].value;

  throw undefine_symbol;
}

SymbolTable::Type SymbolTable::getSymbolType(int value) {
  if (table[value].value != -1) return table[value].addressType;

  throw undefine_symbol;
}
