// 資工三甲 林詠翔
// must to use -std=c++11 or higher version

#include "identifier_table.h"

#include <sstream>
#include <vector>

#include "quadruple.h"
extern int hashtable_size;

IDData idTableData[HASHTABLE_SIZE];
int firstUndefineLine[HASHTABLE_SIZE];
vector<int> undefineGTO[HASHTABLE_SIZE];

string getGTOString(TokenData token) {
  stringstream ss;
  if (token.type != QUADRUPLE_TABLE)
    cout << "id table GTO String Error!" << endl;

  for (int i = 0; i < hashtable_size; i++) {
    if (idTableData[i].pointer == token.value &&
        idTableData[i].type == TYPE_LABEL) {
      ss << idTableData[i].id;
      return ss.str();
    }
  }

  ss << token.value;
  return ss.str();
}

bool hasLabel(TokenData token) {
  if (token.type != QUADRUPLE_TABLE) return false;

  for (int i = 0; i < hashtable_size; i++) {
    if (idTableData[i].pointer == token.value &&
        idTableData[i].type == TYPE_LABEL)
      return true;
  }

  return false;
}

void resetIDTable() {
  fill_n(idTableData, hashtable_size, IDData{"", -1, -1, -1, false});
  fill_n(firstUndefineLine, hashtable_size, -1);
  for (int i = 0; i < hashtable_size; i++) undefineGTO[i].clear();
}

int hasSubRoutineUndefine() {
  for (int i = 0; i < hashtable_size; i++) {
    if (firstUndefineLine[i] != -1) return firstUndefineLine[i];
  }
  return -1;
}

int hash_fucntion(const string& str, int scope) {
  // int result = scope;
  int result = 0;

  // idk the hash function
  for (int i = 0; i < str.length(); i++)
    result = (result + str.at(i)) % hashtable_size;

  return result;
}

int indexStep(int index, int step) { return (index + step) % hashtable_size; }

int getIDIndex(const string& idStr, int scope, bool findDefined) {
  // cout << hashtable_size << endl;
  int index = hash_fucntion(idStr, scope), step = 0;
  // cout << "not check subroutine" << endl;
  int steped;
  while (idTableData[steped = indexStep(index, step)].defined == true) {
    if (step >= hashtable_size) break;
    if (idTableData[steped].id == idStr && idTableData[steped].scope == scope)
      return steped;
    step++;
  }

  return findDefined ? -1 : steped;
}

bool isIDArray(TokenData token) {
  if (token.type == IDENTIFIER_TABLE && hashtable_size > token.value &&
      token.value >= 0 && idTableData[token.value].defined == true)
    return idTableData[token.value].type == TYPE_ARRAY;
  return false;
}

string getIDString(TokenData token) {
  if (token.type == IDENTIFIER_TABLE && hashtable_size > token.value &&
      token.value >= 0 && idTableData[token.value].defined == true)
    return idTableData[token.value].id;
  return "";
}

bool isIDDefined(const string& idStr, int scope) {
  return getIDIndex(idStr, scope, true) != -1;
}

bool isSubroutine(const string& id, int scope, int lineNum) {
  int index = getIDIndex(id, scope, true);
  if (index == -1) {
    // define
    index = defineID(id, scope, -1, SUBROUTINE_UNDEFINE_MARK).value;
    firstUndefineLine[index] = lineNum;
    return true;
  } else {
    // check is subroutine
    // point -1 mean that is main
    // SUBROUTINE_UNDEFINE_MARK is -2
    if (idTableData[index].scope == scope && idTableData[index].type == -1 &&
        idTableData[index].pointer != -1)
      return true;
    else
      return false;
  }
}

TokenData defineSubroutine(const string& id, int scope, int pointer) {
  int index = getIDIndex(id, -1, false);
  idTableData[index] = {id, -1, -1, pointer, true};
  firstUndefineLine[index] = -1;
  return {IDENTIFIER_TABLE, index};
}

TokenData defineID(const string& id, int scope, int type, int pointer) {
  if (isIDDefined(id, scope)) cout << "'" << id << "'defined?" << endl;
  int index = getIDIndex(id, scope, false);

  idTableData[index] = {id, scope, type, pointer, true};

  return {IDENTIFIER_TABLE, index};
}

TokenData getID(const string& id, int scope) {
  int index = getIDIndex(id, scope, true);
  return {IDENTIFIER_TABLE, index};
}

int getPointer(const string& id, int scope) {
  int index = getIDIndex(id, scope, true);
  return idTableData[index].pointer;
}

bool defineLabelStatement(const string& id, int scope, int pointer) {
  if (isIDDefined(id, scope)) {
    int index = getIDIndex(id, scope, true);

    if (idTableData[index].type != TYPE_LABEL) return false;

    if (idTableData[index].pointer != -1) return false;

    idTableData[index].pointer = pointer;

    for (auto it : undefineGTO[index])
      modifyQFormResult(it, {QUADRUPLE_TABLE, pointer});

    undefineGTO[index].clear();
    return true;
  }

  return false;
}

bool referenceLabel(const string& id, int scope, int pointer) {
  if (isIDDefined(id, scope)) {
    int index = getIDIndex(id, scope, true);

    if (!idTableData[index].type == TYPE_LABEL) return false;

    if (idTableData[index].pointer == -1)
      undefineGTO[index].push_back(pointer);
    else
      modifyQFormResult(pointer, {QUADRUPLE_TABLE, idTableData[index].pointer});
    return true;
  }

  return false;
}

bool checkLabel() {
  for (int i = 0; i < hashtable_size; i++)
    if (undefineGTO[i].size() != 0) return false;

  return true;
}
