// 資工三甲 林詠翔
// must to use -std=c++11 or higher version

#include "optab.h"

#include <algorithm>
#include <iostream>

#include "error.h"

using namespace std;

OPTab::OPTab() {}

void OPTab::loadOPTab(const string &optable_name) {
  fin.open(optable_name, ios::in);

  if (!fin) throw Error::loading_failure;

  string command;
  int type, opcode;

  while (fin >> command >> hex >> opcode >> type) {
    command = toUpper(command);

    // cout << command << "_" << opcode << "_" << type << endl;
    table[command] = Data{opcode, type};
  }
}

string OPTab::toUpper(string upper) {
  transform(upper.begin(), upper.end(), upper.begin(), ::toupper);
  return upper;
}

int OPTab::getOPCode(const string &command) {
  unordered_map<string, Data>::iterator it;
  it = table.find(toUpper(command));

  if (it != table.end()) return it->second.opcode;

  return -1;
}

int OPTab::getFormat(const string &command) {
  unordered_map<string, Data>::iterator it;
  it = table.find(toUpper(command));

  if (it != table.end()) return it->second.format;

  return -1;
}
