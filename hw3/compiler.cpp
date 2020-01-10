// 資工三甲 林詠翔
// must to use -std=c++11 or higher version
#include "compiler.h"

#include <iostream>
#include <stack>
#include <vector>

#include "identifier_table.h"
#include "quadruple.h"

stack<int> subroutineStack;

HashTable intTable(INTEGER_TABLE);
HashTable realTable(REAL_NUMBER_TABLE);

string getInteger(TokenData token) { return intTable.get(token); }
string getReal(TokenData token) { return realTable.get(token); }

vector<int> infoTable;
int tmpCounter;

vector<int>& getInfoTable() { return infoTable; }

TokenData getTemper() { return {TEMP_TABLE, tmpCounter++}; }
void resetCompiler() {
  cout << " resetTables() not work" << endl;

  tmpCounter = 0;
  intTable.reset();
  realTable.reset();
  infoTable.clear();
  infoTable.push_back(0);  // let first item from 1 start
  resetIDTable();
  quadrupleReset();
}

int add_integer(const string& intStr) {
  TokenData token = intTable.put(intStr);
  return token.value;
}

int add_real(const string& realStr) {
  TokenData token = realTable.put(realStr);
  return token.value;
}

int add_identifier(const string& idStr) { cout << "id\'" << idStr << '\''; }

int add_arrayInfo(int type, vector<int>& argRegister) {
  int putIndex = infoTable.size();

  infoTable.push_back(type);
  infoTable.push_back(argRegister.size());
  for (auto dim : argRegister) {
#ifdef DEEBUG
    cout << dim << endl;
#endif
    infoTable.push_back(dim);
  }

  return putIndex;
}

int add_callInfo(vector<int>& argRegister) {
  int putIndex = infoTable.size();

  infoTable.push_back(argRegister.size() / 2);
  for (auto data : argRegister) infoTable.push_back(data);

  return putIndex;
}

int get_scope() {
  if (subroutineStack.size() == 0)
    return -1;
  else
    return subroutineStack.top();
}

void push_scope(int value) { subroutineStack.push(value); }

void pop_scope() {
  if (subroutineStack.size() == 0)
    cout << "scope stack error?" << endl;
  else
    subroutineStack.pop();
}
