// 資工三甲 林詠翔
// must to use -std=c++11 or higher version
#include "quadruple.h"

#include <cassert>
#include <cstdio>
#include <sstream>
#include <vector>

#include "compiler.h"
#include "identifier_table.h"
#define GTO_TABLE_VALUE 11

LoadingTable delimiterTable = LoadingTable(DELIMITER_TABLE);
LoadingTable reservedTable = LoadingTable(RESERVED_WORD_TABLE);

vector<QForm> qfromTable;

bool isTopGoto() {
  if (qfromTable.size() > 0 &&
      isTokenEqual(qfromTable.back().opr,
                   {RESERVED_WORD_TABLE, GTO_TABLE_VALUE}))
    return true;
  else
    return false;
}

void quadrupleReset() {
  qfromTable.clear();
  delimiterTable = LoadingTable(DELIMITER_TABLE);
  reservedTable = LoadingTable(RESERVED_WORD_TABLE);
  delimiterTable.loadTable("Table1.table");
  reservedTable.loadTable("Table2.table");
}

int getQFormSize() { return qfromTable.size(); }

int addQForm(QuadrupleForm qform) {
  int index = qfromTable.size();
  qfromTable.push_back(qform);
  return index;
}

void modifyQFormOP1(int index, TokenData token) {
  qfromTable.at(index).opd1 = token;
}

void modifyQFormOP2(int index, TokenData token) {
  qfromTable.at(index).opd2 = token;
}

void modifyQFormResult(int index, TokenData token) {
  qfromTable.at(index).result = token;
}

int getQFormNext() { return getQFormSize() + 1; }

void qformTest() {
  cout << "Qform test" << endl;
  addQForm({NULL_TOKEN, NULL_TOKEN, NULL_TOKEN, NULL_TOKEN});
  modifyQFormOP1(0, {1, 1});
  assert(!isTokenEqual(qfromTable[0].opd1, NULL_TOKEN));
  modifyQFormOP2(0, {1, 1});
  assert(!isTokenEqual(qfromTable[0].opd2, NULL_TOKEN));
  modifyQFormResult(0, {1, 1});
  assert(!isTokenEqual(qfromTable[0].result, NULL_TOKEN));
  printQForm(stdout);
  quadrupleReset();
}

string printToken(TokenData token) {
  stringstream ss;
  if (!isTokenEqual(token, NULL_TOKEN))
    ss << "(" << token.type << "," << token.value << ")";
  return ss.str();
}

string getString(TokenData token) {
  switch (token.type) {
    case TEMP_TABLE:
      return "T" + to_string(token.value + 1);
    case RESERVED_WORD_TABLE:
      return reservedTable.get(token);
    case DELIMITER_TABLE:
      return delimiterTable.get(token);
    case INTEGER_TABLE:
      return getInteger(token);
    case REAL_NUMBER_TABLE:
      return getReal(token);
    case IDENTIFIER_TABLE:
      return getIDString(token);
    case QUADRUPLE_TABLE:
      cout << "Undefine Table to string" << endl;
      return "";
    case INFORMATION_TABLE:
      cout << "Undefine Table to string" << endl;
      return "";
    default:
      cout << "getString error" << endl;
      return "";
  }
}

string printSource(QuadrupleForm qform) {
  stringstream ss;
  TokenData opr = qform.opr, result = qform.result;
  TokenData opd1 = qform.opd1, opd2 = qform.opd2;
  if (!isTokenEqual(opr, NULL_TOKEN)) {
    if (opr.type == IDENTIFIER_TABLE) ss << getString(opr);
    if (isTokenEqual(opr, delimiterTable.get('='))) {
      if (isIDArray(opd1))
        ss << getString(result) << " = " << getString(opd1) << "("
           << getString(opd2) << ")";
      else if (isIDArray(opd2))
        ss << getString(opd2) << "(" << getString(result) << ")"
           << "=" << getString(opd1);
      else
        ss << getString(result) << "=" << getString(opd1);
    } else if (isTokenEqual(opr, reservedTable.get("CALL"))) {
      vector<int> infoTab = getInfoTable();
      int argBegin = result.value;
      int argSize = infoTab.at(argBegin);
      argBegin++;
      ss << "CALL " << getString(opd1) << "(";
      for (int i = 0; i < argSize; i++) {
        if (i != 0) ss << ",";
        ss << getString(
            {infoTab.at(argBegin + i * 2), infoTab.at(argBegin + i * 2 + 1)});
      }
      ss << ")";
    } else if (opr.type == RESERVED_WORD_TABLE) {
      if (opr.value == 12) {  // IF
        ss << getString(opr) << " ";
        ss << getString(opd1) << " ";
        ss << "GO TO " << opd2.value;
        ss << ", ELSE GO TO " << result.value;
      } else if (opr.value == 11) {  // GTO
        ss << getString(opr) << " ";
        ss << getGTOString(result);
      } else if (opr.value == 6 || opr.value == 7)  // ENP ENS
        ss << getString(opr);
      else if (opr.value == 13 || opr.value == 20)  // input output
        ss << getString(opr) << " " << getString(result);
      else {
        ss << getString(result) << " = " << getString(opd1) << " "
           << getString(opr) << " " << getString(opd2);
      }

    } else if (opr.type == DELIMITER_TABLE)
      ss << getString(result) << " = " << getString(opd1) << getString(opr)
         << getString(opd2);
  }

  return ss.str();
}

void printQForm(FILE *fp) {
  int i = 0;
  for (auto form : qfromTable) {
    fprintf(fp, "%-7d(%-7s,%-7s,%-7s,%-7s)%10c%s\n", i,
            printToken(form.opr).c_str(), printToken(form.opd1).c_str(),
            printToken(form.opd2).c_str(), printToken(form.result).c_str(), ' ',
            printSource(form).c_str());
    i++;
  }
}
