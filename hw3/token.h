// 資工三甲 林詠翔
// c only
#ifndef _TOKEN_H_
#define _TOKEN_H_

#define NULL_TOKEN \
  { -1, -1 }

typedef struct TokenData {
  int type, value;
} TokenData;

typedef struct ArrayToken {
  TokenData token, index;
} ArrayToken;

#endif
