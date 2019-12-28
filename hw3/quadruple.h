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
int addQForm(QuadrupleForm qform);
void printQForm(FILE *fp);

#endif
