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
vector<string> intTableMode2, realTableMode2;

bool useHashNum = true;

void enableNumHash(bool hash) { useHashNum = hash; }

string getInteger(TokenData token) {
  if (useHashNum)
    return intTable.get(token);
  else
    return intTableMode2[token.value];
}
string getReal(TokenData token) {
  if (useHashNum)
    return realTable.get(token);
  else
    return realTableMode2[token.value];
}

int add_integer(const string& intStr) {
  TokenData token;
  if (useHashNum)
    token = intTable.put(intStr);
  else {
    for (int i = 0; i < intTableMode2.size(); i++) {
      if (intTableMode2[i] == intStr) return i;
    }
    token.value = intTableMode2.size();
    intTableMode2.push_back(intStr);
  }
  return token.value;
}
void printIntTableMode2() {
  for (auto it : intTableMode2) cout << it << endl;
}

int add_real(const string& realStr) {
  TokenData token;

  if (useHashNum)
    token = realTable.put(realStr);
  else {
    for (int i = 0; i < realTableMode2.size(); i++) {
      if (realTableMode2[i] == realStr) return i;
    }
    token.value = realTableMode2.size();
    realTableMode2.push_back(realStr);
  }
  return token.value;
}

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
  intTableMode2.clear();
  realTableMode2.clear();
  // infoTable.push_back(0);  // let first item from 1 start
  resetIDTable();
  quadrupleReset();
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
  /*
  if (subroutineStack.size() == 0)
    return -1;
  else
    return subroutineStack.top();
    */
  return 0;
}

void push_scope(int value) { subroutineStack.push(value); }

void pop_scope() {
  if (subroutineStack.size() == 0)
    cout << "scope stack error?" << endl;
  else
    subroutineStack.pop();
}
