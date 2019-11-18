// 資工三甲 林詠翔
// must to use -std=c++11 or higher version

#ifndef _LEXICALER_H_
#define _LEXICALER_H_
#include <vector>

#include "table.h"

using namespace std;

class Lexicaler {
  Table &getTable(int index);
  fstream fin;
  string filename, saveName;

  LoadingTable instructionTable = LoadingTable(INSTRUCTION_TABLE);
  LoadingTable pseudoExtraTable = LoadingTable(PSEUDO_EXTRA_TABLE);
  LoadingTable registerTable = LoadingTable(REGISTER_TABLE);
  LoadingTable delimiterTable = LoadingTable(DELIMITER_TABLE);
  HashTable symbolTable = HashTable(SYMBOL_TABLE);
  HashTable integerTable = HashTable(INTEGER_REAL_TABLE);
  HashTable stringTable = HashTable(STRING_TABLE);

  void printTableValue(TokenData);
  bool checkTokenData(const TokenData token);

 public:
  vector<TokenData> lexingLine(const string &line);
  string lexingLineString(const string &line);
  TokenData checkToken(const string &token);
  string tableValueToString(TokenData token);

  Lexicaler();
  void lexing();
  void reset();
  void loadFile(const string &);
  void writeFile(const string &);
  string getData(TokenData);
  enum Mode { mode_normal, mode_string, mode_integer, mode_comment };
};

#endif
