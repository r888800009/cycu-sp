// 資工三甲 林詠翔
// must to use -std=c++11 or higher version
#include "test.h"
#include <cassert>
#include <iostream>
#include "table.h"
using namespace std;

void testHashTable() {
  HashTable table1(1);

  // test null
  TokenData data = table1.get("test");
  assert(data.type == -1 && data.value == -1);

  // test Upper lower
  table1.put("tesT");
  data = table1.get("test");
  assert(data.type == -1 && data.value == -1);

  data = table1.put("test");
  // cout << data.type << ":" << data.value << endl;
  assert(data.type == 1);
  assert(data.value == ('t' + 'e' + 's' + 't') % 100 + 1);

  data = table1.get("test");
  // cout << data.type << ":" << data.value << endl;
  assert(data.type == 1);
  assert(data.value == ('t' + 'e' + 's' + 't') % 100 + 1);

  // collision
  table1.put("ttse");
  data = table1.get("ttse");
  assert(data.type == 1 && data.value == ('t' + 'e' + 's' + 't' + 1) % 100 + 1);

  // some value
  data = table1.get("test");
  TokenData data2 = table1.put("test");
  assert(data.value == data2.value);

  // more then size
  table1.reset();
  for (int i = 0; i < HASHTABLE_SIZE; i++) table1.put(to_string(i));

  // full table test
  table1.reset();
  try {
    for (int i = 0; i < HASHTABLE_SIZE + 1; i++) table1.put(to_string(i));
    assert(false);
  } catch (char const *e) {
  }
}

void test() { testHashTable(); }
