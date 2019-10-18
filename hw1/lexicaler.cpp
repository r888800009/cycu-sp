// 資工三甲 林詠翔
// must to use -std=c++11 or higher version
#include "lexicaler.h"

#include <cassert>
#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>

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
  this->filename = filename;
  cout << "Loading file: \"" << filename << '"' << endl;
  fin.open(this->filename, ios::in);
  if (!fin) {
    cout << "Can't load file " << this->filename << endl;
    throw loading_failure;
  }

  regex postFix("_input.txt$");
  string newName = filename;
  newName = regex_replace(newName, postFix, "");
  this->saveName = newName + "_output.txt";
  cout << "SaveName: \"" << saveName << '"' << endl;
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

void Lexicaler::reset() {
  symbolTable.reset();
  integerTable.reset();
  stringTable.reset();
}

string Lexicaler::getData(TokenData data) {
  switch (data.type) {
    case INSTRUCTION_TABLE:
      return instructionTable.get(data);
    case PSEUDO_EXTRA_TABLE:
      return pseudoExtraTable.get(data);
    case REGISTER_TABLE:
      return registerTable.get(data);
    case DELIMITER_TABLE:
      return delimiterTable.get(data);
    case STRING_TABLE:
      return stringTable.get(data);
    case INTEGER_REAL_TABLE:
      return integerTable.get(data);
    case SYMBOL_TABLE:
      return symbolTable.get(data);
    default:
      assert(false);
  }
}

string Lexicaler::lexingLine(const string& line) {
  string result = "";

  // split
  Mode mode = mode_normal;
  string curStr = "";
  int index = 0;
  while (index < line.size()) {
    char getChar = line.at(index);
    switch (mode) {
      case mode_normal:
        if (isspace(getChar)) {
          if (curStr != "") {
            TokenData data = checkToken(curStr);
            result += tableValueToString(data);
            curStr = "";
          }

          break;
        }

        if (delimiterTable.exist(getChar)) {
          if (getChar == '\'') {
            // into string mode or comment mode
            if (curStr == "x" || curStr == "X")
              mode = mode_integer;
            else if (curStr == "c" || curStr == "C")
              mode = mode_string;
            else
              mode = mode_string;
          } else {
            if (curStr != "") {
              TokenData data = checkToken(curStr);
              result += tableValueToString(data);
            }

            if (getChar == '.') mode = mode_comment;
          }

          TokenData data = delimiterTable.get(getChar);
          result += tableValueToString(data);
          curStr = "";
          break;
        }

        curStr += getChar;

        break;
      case mode_string:
        // until ' back to normal mode
        if (getChar == '\'') {
          result += tableValueToString(stringTable.put(curStr));
          TokenData data = delimiterTable.get(getChar);
          result += tableValueToString(data);
          curStr = "";
          mode = mode_normal;
        } else {
          curStr += getChar;
        }

        break;
      case mode_integer:
        // until ' back to normal mode
        if (getChar == '\'') {
          result += tableValueToString(integerTable.put(curStr));
          TokenData data = delimiterTable.get(getChar);
          result += tableValueToString(data);
          curStr = "";
          mode = mode_normal;
        } else {
          curStr += getChar;
        }
        break;
      case mode_comment:
        // cout << "?";
        // until next line
        break;
      default:
        assert(false);
    }

    index++;
  }

  // last token
  if (curStr != "") {
    TokenData data = checkToken(curStr);
    result += tableValueToString(data);
    curStr = "";
  }

  return result;
}

void Lexicaler::lexing() {
  // loading file
  string line, token;
  fstream fout;

  fout.open(saveName, ios::out);
  if (!fout) {
    cout << "Can't open file " << saveName << endl;
    throw saving_failure;
  }

  cout << "start write: \"" << saveName << '"' << endl;
  while (getline(fin, line)) {
    // get line
    cout << line << endl;
    fout << line << endl;

    string out = lexingLine(line);
    cout << out << endl;
    fout << out << endl;
  }

  fout.close();
  fin.close();
}

