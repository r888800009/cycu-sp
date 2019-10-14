// 資工三甲 林詠翔
// must to use -std=c++11 or higher version
#include <cassert>
#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>

#include "lexicaler.h"

using namespace std;

Lexicaler::Lexicaler() {
  instructionTable.loadTable("Table1.table");
  pseudoExtraTable.loadTable("Table2.table");
  registerTable.loadTable("Table3.table");
  delimiterTable.loadTable("Table4.table");
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

void Lexicaler::printTableValue(TokenData token) {
  cout << "(" << token.type << "," << token.value << ")";
}

string Lexicaler::tableValueToString(TokenData token) {
  stringstream result;
  result << "(" << token.type << "," << token.value << ")";
  return result.str();
}

bool Lexicaler::checkTokenData(const TokenData token) {
  if (token.type == -1 || token.value == -1) return false;
  return true;
}

TokenData Lexicaler::checkToken(const string& token) {
  if (instructionTable.exist(token))
    return instructionTable.get(token);
  else if (pseudoExtraTable.exist(token))
    return pseudoExtraTable.get(token);
  else if (registerTable.exist(token))
    return registerTable.get(token);
  else if (regex_match(token, regex("[0-9]+")))
    return integerTable.put(token);
  else
    return symbolTable.put(token);
  return {-1, -1};
}

void Lexicaler::lexing() {
  // loading file
  string line, token;
  while (getline(fin, line)) {
    stringstream spliter;
    // get line
    spliter << line;
    cout << line << endl;

    // split
    Mode mode = mode_normal;
    string curStr = "";
    while (spliter >> token) {
      switch (mode) {
        case mode_normal:
          // into string mode or comment mode
          break;
        case mode_string:
          // until ' back to normal mode
          break;
        case mode_comment:
          // until next line
          break;
        default:
          assert(false);
      }

      TokenData tokenData = {-1, -1};

      if (checkTokenData(tokenData = instructionTable.get(token)))
        printTableValue(tokenData);
      else if (checkTokenData(tokenData = pseudoExtraTable.get(token)))
        printTableValue(tokenData);
      else if (checkTokenData(tokenData = registerTable.get(token)))
        printTableValue(tokenData);
      else if (checkTokenData(tokenData = delimiterTable.get(token))) {
        // check next token
        // throw
        // print token
        printTableValue(tokenData);
      } else {
        cout << " " << token << " ";
      }
    }
    cout << endl;
  }
}

