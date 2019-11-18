// 資工三甲 林詠翔
// must to use -std=c++11 or higher version
#include "table.h"

#include <algorithm>
#include <cassert>
#include <string>

#include "error.h"

using namespace std;

int Table::getTableID() {
  assert(tableID >= 1);
  return tableID;
}

HashTable::HashTable(int tableID) {
  this->tableID = tableID;
  reset();
}

void HashTable::reset() {
  for (int i = 0; i < HASHTABLE_SIZE; i++) {
    tableData[i].empty = true;
    tableData[i].value = "";
  }
}

LoadingTable::LoadingTable(int tableID) { this->tableID = tableID; }

void LoadingTable::loadTable(const string& filename) {
  cout << "Loading table: " << filename << endl;
  this->filename = filename;

  // open file
  fin.open(filename, ios::in | ios::binary);
  if (!fin) {
    cout << "Can't load table " << filename << endl;
    throw Error::loading_failure;
  }

  // loading file
  int counter = 1;
  while (!fin.eof()) {
    string line;
    fin >> line;  // loading and trim
    transform(line.begin(), line.end(), line.begin(), ::toupper);
    tableData[line] = counter;
    revTableData[counter] = line;
    counter++;
  }
}

TokenData LoadingTable::get(char delimiter) {
  string token(1, delimiter);
  return get(token);
}

TokenData LoadingTable::get(const string& token) {
  int result;
  unordered_map<string, int>::iterator it;
  string upper = token;

  transform(upper.begin(), upper.end(), upper.begin(), ::toupper);
  it = tableData.find(upper);

  if (it != tableData.end()) {
    result = it->second;
    assert(tableData.size() >= result && result >= 1);
    return {this->tableID, result};
  }

  return {-1, -1};
}

bool LoadingTable::exist(const string& token) {
  TokenData tokenData = get(token);
  if (tokenData.value != -1) return true;
  return false;
}

bool LoadingTable::exist(char delimiter) {
  string token(1, delimiter);
  return exist(token);
}

int HashTable::hash_fucntion(const string& str) {
  int result = 0;

  for (int i = 0; i < str.length(); i++)
    result = (result + str.at(i)) % HASHTABLE_SIZE;

  return result;
}

int HashTable::indexStep(int index, int step) {
  return (index + step) % HASHTABLE_SIZE;
}

TokenData HashTable::put(const string& token) {
  int index = hash_fucntion(token), step = 0;

  while (tableData[indexStep(index, step)].empty == false) {
    if (step >= HASHTABLE_SIZE) throw "Full table";
    if (tableData[indexStep(index, step)].value == token)
      return {this->tableID, indexStep(index, step)};
    step++;
  }

  tableData[indexStep(index, step)].empty = false;
  tableData[indexStep(index, step)].value = token;

  return {this->tableID, indexStep(index, step)};
}

string LoadingTable::get(TokenData data) {
  if (data.type == tableID) return revTableData[data.value];
  return "";
}

string HashTable::get(TokenData data) {
  if (data.type == tableID) return tableData[data.value].value;
  return "";
}

TokenData HashTable::get(const string& token) {
  int index = hash_fucntion(token), step = 0;

  while (tableData[indexStep(index, step)].empty == false) {
    if (step >= HASHTABLE_SIZE) break;
    if (tableData[indexStep(index, step)].value == token)
      return {this->tableID, indexStep(index, step)};
    step++;
  }
  return {-1, -1};
}
