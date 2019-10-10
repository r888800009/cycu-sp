// 資工三甲 林詠翔
// must to use -std=c++11 or higher version
#include "table.h"
#include <algorithm>
#include <cassert>
#include <string>

using namespace std;

int Table::getTableID() {
  assert(tableID >= 1);
  return tableID;
}

LoadingTable::LoadingTable(int tableID) { this->tableID = tableID; }

void LoadingTable::loadTable(const string& filename) {
  cout << "Loading table: " << filename << endl;
  this->filename = filename;

  // open file
  fin.open(filename, ios::in | ios::binary);
  if (!fin) {
    cout << "Can't load table " << filename << endl;
    throw loading_failure;
  }

  // loading file
  int counter = 1;
  while (!fin.eof()) {
    string line;
    fin >> line;  // loading and trim
    transform(line.begin(), line.end(), line.begin(), ::toupper);
    tableData[line] = counter;
    counter++;
  }
}

TokenData LoadingTable::get(const string& token) {
  int result;
  map<string, int>::iterator it;
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
