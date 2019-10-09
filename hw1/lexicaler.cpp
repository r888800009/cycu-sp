// 資工三甲 林詠翔
// must to use -std=c++11 or higher version
#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>

#include "lexicaler.h"

using namespace std;

Lexicaler::Lexicaler() {
  tables.push_back(Table());                       // null table
  tables.push_back(LoadingTable("Table1.table"));  // Instruction Table 1
  tables.push_back(LoadingTable("Table2.table"));  // Pseudo and Extra Table 2
  tables.push_back(LoadingTable("Table3.table"));  // Register table 3
  tables.push_back(LoadingTable("Table4.table"));  // Delimiter Table 4
  // Symbol Table 5
  // Integer/Real Table 6
  // String Table 7
}

void Lexicaler::loadFile(const string& filename) {
  cout << "Loading file: \"" << filename << '"' << endl;

  fin.open(filename, ios::in | ios::binary);
  if (!fin) {
    cout << "Can't load file " << filename << endl;
    throw loading_failure;
  }
}

Table& Lexicaler::getTable(int index) { assert(index >= 1); }

void Lexicaler::lexing() {
  // loading file
  string line, token;
  while (getline(fin, line)) {
    stringstream spliter;
    // get line
    spliter << line;
    cout << line << endl;

    // split
    while (spliter >> token) cout << " " << token;
    cout << endl;
  }
}

