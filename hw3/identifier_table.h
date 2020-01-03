// 資工三甲 林詠翔
// must to use -std=c++11 or higher version
#ifndef _IDENTIFIER_TABLE_H_
#define _IDENTIFIER_TABLE_H_

#include <algorithm>
#include <string>

#include "compiler.h"
#include "table.h"

using namespace std;
typedef struct IDData {
  string id;
  int scope;
  int type;
  int pointer;
  bool defined;
} IDData;

#define SUBROUTINE_UNDEFINE_MARK -2

void resetIDTable();
bool isIDDefined(const string& idStr, int scope);
bool isSubroutine(const string& id, int scope, int lineNum);
bool isIDArray(TokenData token);
TokenData defineID(const string& id, int scope, int type, int pointer);
TokenData defineSubroutine(const string& id, int scope, int pointer);
string getIDString(TokenData token);
TokenData getID(const string& id, int scope);

int getPointer(const string& id, int scope);

int hasSubRoutineUndefine();
string getGTOString(TokenData token);
bool hasLabel(TokenData token);

#endif
