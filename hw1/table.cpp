// 資工三甲 林詠翔
// must to use -std=c++11 or higher version
#include "table.h"
#include <cassert>

using namespace std;

LoadingTable::LoadingTable(const string& filename) {
  this->filename = filename;
  loadtable(filename);
}

void LoadingTable::loadtable(const string& filename) {
  cout << "Loading table: " << filename << endl;

  // open file
  fin.open(filename, ios::in | ios::binary);
  if (!fin) {
    cout << "Can't load table " << filename << endl;
    throw loading_failure;
  }

  // loading file
}

int LoadingTable::get(const string&) {
  int result;

  assert(result >= 1);
  return result;
}
