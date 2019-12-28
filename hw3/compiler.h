// 資工三甲 林詠翔
// must to use -std=c++11 or higher version
#ifndef _COMPILER_H_
#define _COMPILER_H_

#define TYPE_ARRAY 1
#define TYPE_BOOLEAN 2
#define TYPE_CHARACTER 3
#define TYPE_INTEGER 4
#define TYPE_LABEL 5
#define TYPE_REAL 6

#include <string>
#include <vector>

#include "table.h"

using namespace std;
void resetCompiler();

int add_integer(const string& intStr);
int add_real(const string& realStr);
int add_identifier(const string& idStr);

TokenData getTemper();

int get_scope();
void push_scope(int value);
void pop_scope();

vector<int>& getInfoTable();
int add_arrayInfo(int type, vector<int>& argRegister);
int add_callInfo(vector<int>& argRegister);
string getInteger(TokenData token);
string getReal(TokenData token);

#endif
