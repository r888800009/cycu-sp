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
  if (table[value].addressType == absolute_address) throw duplicate_define;

  table[value] = {address, type};
}

void SymbolTable::update(int value, int address, Type type) {
  if (table[value].addressType == null_symbol) throw undefine_symbol;

  table[value] = {address, type};
}

int SymbolTable::getSymbolAddress(int value) {
  if (table[value].addressType != null_symbol) return table[value].value;

  throw undefine_symbol;
}

SymbolTable::Type SymbolTable::getSymbolType(int value) {
  if (table[value].addressType != null_symbol) return table[value].addressType;

  throw undefine_symbol;
}
