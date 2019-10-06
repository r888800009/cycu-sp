// 資工三甲 林詠翔
// must to use -std=c++11 or higher version
#include <fstream>
#include <iostream>

#include "lexicaler.h"

Lexicaler::Lexicaler() {}

void Lexicaler::loadFile(const string& filename) {
  cout << "Loading file: \"" << filename << '"' << endl;
  fstream fin;

  fin.open(filename, ios::in | ios::binary);
  if (!fin) {
    cout << "Can't load file " << filename << endl;
    throw loading_failure;
  }
}
