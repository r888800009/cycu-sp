// 資工三甲 林詠翔
// must to use -std=c++11 or higher version
#ifndef _TABLE_H_
#define _TABLE_H_

using namespace std;

class table {};

class LoadingTable : table {
 public:
  void loadtable(const string&);

  int get(const string&);
};

class HashTable : table {
  int hash_fucntion(const string&);

 public:
};

#endif
