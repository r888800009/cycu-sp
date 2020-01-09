// 資工三甲 林詠翔
// must to use -std=c++11 or higher version
#ifndef _QUADRUPLE_TABLE_H_
#define _QUADRUPLE_TABLE_H_

#include "table.h"

typedef struct QuadrupleForm {
  TokenData opr;         // operator
  TokenData opd1, opd2;  // operand
  TokenData result;
} QForm;

void quadrupleReset();
int getQFormSize();
int getQFormNext();
int addQForm(QuadrupleForm qform);
bool isTopGoto();
void printQForm(FILE *fp);

void modifyQFormOP1(int index, TokenData token);
void modifyQFormOP2(int index, TokenData token);
void modifyQFormResult(int index, TokenData token);

void qformTest();
#endif
